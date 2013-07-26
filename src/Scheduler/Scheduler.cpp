/*
 * Copyright (c) 2013 Ghrum Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <Scheduler/Scheduler.hpp>
#include <chrono>

using namespace Ghrum;

/////////////////////////////////////////////////////////////////
// {@see Scheduler::Scheduler} //////////////////////////////////
/////////////////////////////////////////////////////////////////
Scheduler::Scheduler()
    : active_(true), overloaded_(false), uptime_(0), iterationPerSecond_(60),
      thread_(boost::thread::hardware_concurrency()) {
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::runMainThread} //////////////////////////////
/////////////////////////////////////////////////////////////////
void Scheduler::runMainThread() {
    std::queue<std::shared_ptr<Task>> syncronizedQueue;
    std::chrono::steady_clock::time_point endClock, diffClock;
    size_t differenceTime = 0;

    // Turn on all workers inside the worker group pool.
    BOOST_LOG_TRIVIAL(info)
            << "[*] <Scheduler> Executing with " << thread_ << " workers.";
    workerGroup_.start(thread_);

    // Start the clock of the scheduler.
    std::chrono::steady_clock::time_point startClock
        = std::chrono::steady_clock::now();

    // Run the main scheduler.
    do {
        // Push every parallel task in the current tick
        // and get all syncronized task ready to be executed.
        {
            // =================== Lock ===================
            boost::mutex::scoped_lock lock(mutex_);
            // =================== Lock ===================
            if (!taskQueue_.empty())
                runTaskParallel(syncronizedQueue);
        }

        // Run the syncronized task along if there is any task
        // to be executed.
        if (!syncronizedQueue.empty()) {
            runTaskQueue(syncronizedQueue);
        }

        // Check the timing of the scheduler and populate
        // accounting information.
        if (++uptime_ < nextTick_) {
            continue;
        }

        nextTick_ += iterationPerSecond_;
        endClock   = std::chrono::steady_clock::now();
        differenceTime
            = std::chrono::duration_cast<std::chrono::milliseconds>(endClock - startClock).count();
        overloaded_ = (differenceTime >= MILLISECOND_TICK);
        if (!overloaded_) {
            boost::this_thread::sleep(
                boost::posix_time::milliseconds(MILLISECOND_TICK - differenceTime));
        }
        startClock = std::chrono::steady_clock::now();
    } while (active_);

    // Finally before returning control to the user, stop
    // every worker created, by waiting for their last completation
    // handler.
    workerGroup_.joinAll();
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::isActive} ///////////////////////////////////
/////////////////////////////////////////////////////////////////
bool Scheduler::isActive() {
    return active_;
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::isOverloaded} ///////////////////////////////
/////////////////////////////////////////////////////////////////
bool Scheduler::isOverloaded() {
    return overloaded_;
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::getUptime} //////////////////////////////////
/////////////////////////////////////////////////////////////////
size_t Scheduler::getUptime() {
    return uptime_;
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::getThreadCount} /////////////////////////////
/////////////////////////////////////////////////////////////////
size_t Scheduler::getThreadCount() {
    return thread_;
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::getIterationPerSecond} //////////////////////
/////////////////////////////////////////////////////////////////
size_t Scheduler::getIterationPerSecond() {
    return iterationPerSecond_;
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::setIterationPerSecond} //////////////////////
/////////////////////////////////////////////////////////////////
void Scheduler::setIterationPerSecond(size_t iteration) {
    iterationPerSecond_ = iteration;
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::cancel} /////////////////////////////////////
/////////////////////////////////////////////////////////////////
void Scheduler::cancel(IPlugin & owner) {
    for (auto task : taskQueue_)
        if (task->getOwner() != nullptr && task->getOwner()->getId() == owner.getId())
            task->setCancelled();
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::cancelAll} //////////////////////////////////
/////////////////////////////////////////////////////////////////
void Scheduler::cancelAll() {
    for (auto task : taskQueue_)
        task->setCancelled();
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::syncRepeatingTask} //////////////////////////
/////////////////////////////////////////////////////////////////
ITask & Scheduler::syncRepeatingTask(IPlugin & owner, Delegate<void()> callback, TaskPriority priority, uint32_t delay,
                                     uint32_t period) {
    // =================== Lock ===================
    boost::mutex::scoped_lock lock(mutex_);
    // =================== Lock ===================

    std::shared_ptr<Task> task
        = std::make_shared<Task>(&owner, callback, period, false);
    task->setTickTime(uptime_ + delay, overloaded_);
    task->setPriority(priority);
    taskQueue_.push(task);
    return static_cast<ITask &>(*task);
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::asyncDelayedTask} ///////////////////////////
/////////////////////////////////////////////////////////////////
ITask & Scheduler::asyncDelayedTask(IPlugin & owner, Delegate<void()> callback, TaskPriority priority, uint32_t delay) {
    // =================== Lock ===================
    boost::mutex::scoped_lock lock(mutex_);
    // =================== Lock ===================

    std::shared_ptr<Task> task
        = std::make_shared<Task>(&owner, callback, 0, true);
    task->setTickTime(uptime_ + delay, overloaded_);
    task->setPriority(priority);
    taskQueue_.push(task);
    return static_cast<ITask &>(*task);
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::asyncAnonymousTask} /////////////////////////
/////////////////////////////////////////////////////////////////
ITask & Scheduler::asyncAnonymousTask(Delegate<void()> callback, TaskPriority priority) {
    // =================== Lock ===================
    boost::mutex::scoped_lock lock(mutex_);
    // =================== Lock ===================

    std::shared_ptr<Task> task
        = std::make_shared<Task>(nullptr, callback, 0, true);
    task->setTickTime(uptime_, overloaded_);
    task->setPriority(priority);
    taskQueue_.push(task);
    return static_cast<ITask &>(*task);
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::runTaskParallel} ////////////////////////////
/////////////////////////////////////////////////////////////////
void Scheduler::runTaskParallel(std::queue<std::shared_ptr<Task>> & queue) {
    bool isExecuted = true;
    do {
        // Peek the first task and the one that is closest
        // to be executed.
        const std::shared_ptr<Task> & task = taskQueue_.top();
        isExecuted = (uptime_ >= task->getTickTime());

        // Check if the task can be executed.
        if (isExecuted && task->isAlive()) {
            if (task->isParallel()) {
                workerGroup_.push(task);
            } else {
                queue.push(task);
            }
            taskQueue_.pop();
        } else if (!task->isAlive()) {
            taskQueue_.pop();
        }
    } while (isExecuted && !taskQueue_.empty());
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::runTaskQueue} ///////////////////////////////
/////////////////////////////////////////////////////////////////
void Scheduler::runTaskQueue(std::queue<std::shared_ptr<Task>> & queue) {
    do {
        // Gets the task from the queue and execute its
        // delegate.
        const std::shared_ptr<Task> & task = queue.front();
        static_cast<Task>(*task)();
        task->setTickTime(uptime_, overloaded_);

        // after executing the task, if the task was marked to repeat,
        // then add it back into the task priority quere, otherwise stop it.
        if (task->isAlive() && task->isReapeating()) {
            // =================== Lock ===================
            boost::mutex::scoped_lock lock(mutex_);
            // =================== Lock ===================
            taskQueue_.push(task);
        }

        // Remove the task from the queue since it was already
        // parsed.
        queue.pop();
    } while (!queue.empty());
}
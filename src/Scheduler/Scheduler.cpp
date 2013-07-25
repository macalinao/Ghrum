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
#include <thread>

using namespace Ghrum;

/////////////////////////////////////////////////////////////////
// {@see Scheduler::Scheduler} //////////////////////////////////
/////////////////////////////////////////////////////////////////
Scheduler::Scheduler()
    : active_(false), overloaded_(false), uptime_(0), iterationPerSecond_(60),
      thread_(boost::thread::hardware_concurrency()) {
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::execute} ////////////////////////////////////
/////////////////////////////////////////////////////////////////
void Scheduler::execute() {
    std::queue<std::shared_ptr<Task>> queue;
    size_t nextTickTime = iterationPerSecond_;
    auto start = std::chrono::steady_clock::now();

    // Start the execution of the scheduler.
    active_ = true;

    // Run the scheduler as long as it is active
    // and enabled.
    do {
        // =================== Lock ===================
        mutex_.lock();
        // =================== Lock ===================

        // Execute the first and all the available task
        // on the current tick, if the first function
        // cannot be executed, then don't execute others
        // since the first task to pop from the queue is the
        // first one available to be executed.
        bool isExecuted = true;
        while (isExecuted && !tasks_.empty()) {
            // Peek the first task and the one that is closest
            // to be executed.
            const std::shared_ptr<Task> & task = tasks_.top();
            isExecuted = (uptime_ >= task->getTickTime());

            // Check if the task can be executed.
            if (isExecuted && task->isAlive()) {
                if (task->isParallel()) {
                    workerGroup_.push(
                        TaskWrapper(task));
                } else {
                    queue.push(task);
                }
                tasks_.pop();
            } else if (!task->isAlive())
                tasks_.pop();
        }

        // =================== Unlock ===================
        mutex_.unlock();
        // =================== Unlock ===================

        // Execute all syncronized task availabled found previusly,
        // if the scheduler is overloaded, then it will deferred tasks acording
        // to the task priority given for each task, higher level, means lower
        // deferring time.
        while (!queue.empty()) {
            // Get the task and execute the handler and execute
            // the task handler.
            const std::shared_ptr<Task> & task = queue.front();
            static_cast<Task>(*task)();
            task->setTickTime(uptime_, overloaded_);

            // after executing the task, if the task was marked to repeat,
            // then add it back into the task priority quere, otherwise stop it.
            if (task->isAlive() && task->isReapeating()) {
                // =================== Lock ===================
                std::lock_guard<std::mutex> lock(mutex_);
                // =================== Lock ===================
                tasks_.push(task);
            }
            queue.pop();
        }

        // Check if the tick by second has passed and sleep.
        if (++uptime_ == nextTickTime) {

            // Update the next ticking values, the next check will
            // be LAST_TICK + TICK_PER_SECOND.
            nextTickTime = uptime_ + iterationPerSecond_;

            // Check how much left we have.
            auto end = std::chrono::steady_clock::now();
            auto difference = end - start;
            auto time = std::chrono::duration_cast<std::chrono::milliseconds>(difference).count();
            if (time >= MILLISECOND_TICK) {
                overloaded_ = true;
            } else {
                overloaded_ = false;
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(MILLISECOND_TICK - time));
            }
            start = std::chrono::steady_clock::now();
        }
    } while (active_);

    // Wait and dispose every thread created.
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
    for (auto task : tasks_)
        if (task->getOwner() != nullptr && task->getOwner()->getId() == owner.getId())
            task->setCancelled();
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::cancelAll} //////////////////////////////////
/////////////////////////////////////////////////////////////////
void Scheduler::cancelAll() {
    for (auto task : tasks_)
        task->setCancelled();
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::syncRepeatingTask} //////////////////////////
/////////////////////////////////////////////////////////////////
ITask & Scheduler::syncRepeatingTask(IPlugin & owner, Delegate<void()> callback, TaskPriority priority, uint32_t delay,
                                     uint32_t period) {
    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(mutex_);
    // =================== Lock ===================

    std::shared_ptr<Task> task
        = std::make_shared<Task>(&owner, callback, period, false);
    task->setTickTime(uptime_ + delay, overloaded_);
    task->setPriority(priority);
    tasks_.push(task);
    return static_cast<ITask &>(*task);
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::asyncDelayedTask} ///////////////////////////
/////////////////////////////////////////////////////////////////
ITask & Scheduler::asyncDelayedTask(IPlugin & owner, Delegate<void()> callback, TaskPriority priority, uint32_t delay) {
    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(mutex_);
    // =================== Lock ===================

    std::shared_ptr<Task> task
        = std::make_shared<Task>(&owner, callback, 0, true);
    task->setTickTime(uptime_ + delay, overloaded_);
    task->setPriority(priority);
    tasks_.push(task);
    return static_cast<ITask &>(*task);
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::asyncAnonymousTask} /////////////////////////
/////////////////////////////////////////////////////////////////
ITask & Scheduler::asyncAnonymousTask(Delegate<void()> callback, TaskPriority priority) {
    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(mutex_);
    // =================== Lock ===================

    std::shared_ptr<Task> task
        = std::make_shared<Task>(nullptr, callback, 0, true);
    task->setTickTime(uptime_, overloaded_);
    task->setPriority(priority);
    tasks_.push(task);
    return static_cast<ITask &>(*task);
}
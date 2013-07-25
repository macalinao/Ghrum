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
    : workerGroup_(
        boost::thread::hardware_concurrency() * SCHEDULER_THREAD_FACTOR) {
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::execute} ////////////////////////////////////
/////////////////////////////////////////////////////////////////
void Scheduler::execute() {
    std::queue<std::shared_ptr<Task>> queue;
    size_t nextTickTime = SCHEDULER_TICK_PER_SECOND;
    auto start = std::chrono::steady_clock::now();

    // Start the execution of the scheduler.
    active_ = true;

    // Run the scheduler as long as it is active
    // and enabled.
    while (active_) {
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
            if (isExecuted) {
                tasks_.pop();
                // If the task is parallel, then schedule to run
                // it off over a parallel task, parallel task are
                // not locked into the main thread, if the task
                // is syncronized, then push it back into a syncronized
                // task to be schedule on the main thread after executing
                // all available tasks.
                if (task->isParallel()) {
                    workerGroup_.push(TaskHandler(task));
                } else {
                    queue.push(task);
                }
            } else if (task->isAlive() == false) {
                tasks_.pop();
            }
        }

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
                tasks_.push(task);
            }
            queue.pop();
        }

        // Check if the tick by second has passed and sleep.
        if (++uptime_ == nextTickTime) {

            // Update the next ticking values, the next check will
            // be LAST_TICK + TICK_PER_SECOND.
            nextTickTime = uptime_ + SCHEDULER_TICK_PER_SECOND;

            // Check how much left we have.
            auto end = std::chrono::steady_clock::now();
            auto difference = end - start;
            auto time = std::chrono::duration_cast<std::chrono::milliseconds>(difference).count();

            if (time >= SCHEDULER_TICK_SECOND) {
                overloaded_ = true;
            } else {
                overloaded_ = false;
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(SCHEDULER_TICK_SECOND - time));
            }
            start = std::chrono::steady_clock::now();
        }
    }
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
    return boost::thread::hardware_concurrency() * SCHEDULER_THREAD_FACTOR;
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::getIterationPerSecond} //////////////////////
/////////////////////////////////////////////////////////////////
size_t Scheduler::getIterationPerSecond() {
    return SCHEDULER_TICK_PER_SECOND;
}

/////////////////////////////////////////////////////////////////
// {@see Scheduler::cancel} /////////////////////////////////////
/////////////////////////////////////////////////////////////////
void Scheduler::cancel(IPlugin & owner) {
    for (auto task : tasks_)
        if (task->getOwner() != nullptr
                && task->getOwner()->getId() == owner.getId())
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
// {@see Scheduler::Scheduler} //////////////////////////////////
/////////////////////////////////////////////////////////////////
ITask & Scheduler::addTask(IPlugin * owner, Delegate<void()> callback, TaskPriority priority, uint32_t delay, uint32_t period,
                           bool isParallel) {
    // Create a new pointer to the new task, the task will be disposed by a
    // auto shared_ptr after its released from the container.
    std::shared_ptr<Task> task = std::make_shared<Task>(owner, callback, period, isParallel);
    task->setTickTime(uptime_ + delay, overloaded_);
    task->setPriority(priority);
    tasks_.push(task);
    return static_cast<ITask &>(*task);
}

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
#ifndef _SCHEDULER_HPP_
#define _SCHEDULER_HPP_

#include "Task.hpp"
#include "SchedulerWorkerGroup.hpp"
#include <Scheduler/IScheduler.hpp>
#include <boost/heap/priority_queue.hpp>

namespace Ghrum {

/**
 * Define the thread core factor.
 */
#define SCHEDULER_THREAD_FACTOR 3

/**
 * Declare a second.
 */
#define SCHEDULER_TICK_SECOND 1000

/**
 * Define number of ticks per second.
 */
#define SCHEDULER_TICK_PER_SECOND 60

/**
 * Define the normal time that should consume per TICK_SECOND
 */
#define SCHEDULER_TIME SCHEDULER_TICK_SECOND / SCHEDULER_TICK_PER_SECOND

/**
 * Implementation of {@see IScheduler}.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class Scheduler : public IScheduler {
public:
    /**
     * Default constructor.
     */
    Scheduler();

    /**
     * Start the execution of the scheduler.
     */
    void execute();

    /**
     * {@inheritDoc}
     */
    bool isActive();

    /**
     * {@inheritDoc}
     */
    bool isOverloaded();

    /**
     * {@inheritDoc}
     */
    size_t getUptime();

    /**
     * {@inheritDoc}
     */
    size_t getThreadCount();

    /**
     * {@inheritDoc}
     */
    size_t getIterationPerSecond();

    /**
     * {@inheritDoc}
     */
    void cancel(IPlugin & owner);

    /**
     * {@inheritDoc}
     */
    void cancelAll();

    /**
     * {@inheritDoc}
     */
    ITask & addTask(IPlugin * owner, Delegate<void()> callback, TaskPriority priority, uint32_t delay, uint32_t period,
                    bool isParallel);
protected:
    bool active_, overloaded_;
    size_t uptime_;
    SchedulerWorkerGroup workerGroup_;
    boost::heap::priority_queue<std::shared_ptr<Task>> tasks_;
};

}; // namespace Ghrum

#endif // _SCHEDULER_HPP_
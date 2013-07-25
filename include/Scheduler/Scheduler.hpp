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
    void setIterationPerSecond(size_t iterations);

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
    ITask & syncRepeatingTask(IPlugin & owner, Delegate<void()> callback,TaskPriority priority, uint32_t delay,
                              uint32_t period);

    /**
     * {@inheritDoc}
     */
    ITask & asyncDelayedTask(IPlugin & owner, Delegate<void()> callback, TaskPriority priority, uint32_t delay);

    /**
     * {@inheritDoc}
     */
    ITask & asyncAnonymousTask(Delegate<void()> callback, TaskPriority priority);
protected:
    bool active_, overloaded_;
    size_t uptime_, thread_, iterationPerSecond_;
    SchedulerWorkerGroup workerGroup_;
    std::mutex mutex_;
    boost::heap::priority_queue<std::shared_ptr<Task>> tasks_;
};

}; // namespace Ghrum

#endif // _SCHEDULER_HPP_
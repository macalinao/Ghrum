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
public:
    /**
     * {@inheritDoc}
     */
    void execute();

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
    Task & addTask(IPlugin * owner, Task::Function callback, TaskPriority priority, uint32_t delay,
                   uint32_t period, bool isParallel);
protected:
    SchedulerWorkerGroup workerGroup_;
    boost::heap::priority_queue<std::shared_ptr<Task>> tasks_;
    std::vector<std::shared_ptr<Task>> taskAlive_;
};

}; // namespace Ghrum

#endif // _SCHEDULER_HPP_
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
#ifndef _TASK_HPP_
#define _TASK_HPP_

#include <Scheduler/ITask.hpp>
#include <Utilities/Delegate.hpp>

namespace Ghrum {

/**
 * Implementation of {@see ITask}.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class Task : public ITask {
public:
    /**
     * Default constructor of a task.
     *
     * @param owner the owner of the task
     * @param callback callback
     * @param period the period
     * @param isParallel if the task is parallel
     */
    Task(IPlugin * owner, Delegate<void()> callback, size_t period, bool isParallel);

    /**
     * Override < operator, for comparing task inside
     * containers.
     *
     * @param rhs the other task to compare with
     */
    bool operator<(const Task & rhs);

    /**
     * Override () operator, to handle calling into
     * the task delegate.
     *
     * @param rhs the other task to compare with
     */
    void operator()();

    /**
     * Sets the next tick time to execute.
     *
     * @param current current scheduler tick
     * @param isDeferred if the task should be deferred
     */
    void setTickTime(size_t current, bool isDeferred);

    /**
     * Gets the next tick time that the task will
     * execute.
     */
    size_t getTickTime();

    /**
     * {@inheritDoc}
     */
    IPlugin const * getOwner() const;

    /**
     * {@inheritDoc}
     */
    const std::string & getName() const;

    /**
     * {@inheritDoc}
     */
    void setName(const std::string & name);

    /**
     * {@inheritDoc}
     */
    void setPriority(TaskPriority priority);

    /**
     * {@inheritDoc}
     */
    void setCancelled();

    /**
     * {@inheritDoc}
     */
    bool isAlive();

    /**
     * {@inheritDoc}
     */
    bool isParallel();

    /**
     * {@inheritDoc}
     */
    bool isReapeating();
protected:
    std::string name_;
    IPlugin * owner_;
    TaskPriority priority_;
    size_t tick_, period_;
    Delegate<void()> function_;
    bool active_, parallel_, repeating_;
};

} // namespace Ghrum

#endif // _TASK_HPP_
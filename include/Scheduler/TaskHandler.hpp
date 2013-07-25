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
#ifndef _TASK_HANDLER_HPP_
#define _TASK_HANDLER_HPP_

#include "Task.hpp"

namespace Ghrum {

/**
 * Encapsulate a wrapper handler for any parallel task.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class TaskHandler {
public:
    /**
     * Default constructor of the wrapper handler.
     *
     * @param task a shared pointer to the task handler
     */
    TaskHandler(std::shared_ptr<Task> task)
        : task_(task) {
    }

    /**
     * Define the call operator.
     */
    inline void operator()() {
        static_cast<Task>(*task_)();
    }
private:
    std::shared_ptr<Task> task_;
};

}; // namespace Ghrum

#endif // _TASK_HANDLER_HPP_
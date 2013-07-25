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
#ifndef _TASK_WORKER_GROUP_HPP_
#define _TASK_WORKER_GROUP_HPP_

#include "Task.hpp"
#include "TaskWorker.hpp"

namespace Ghrum {

/**
 * Define a pool of {@see TaskWorker}.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class TaskWorkerGroup {
public:
    /**
     * Default constructor of the group.
     */
    TaskWorkerGroup();

    /**
     * Destructor of the group.
     */
    ~TaskWorkerGroup();

    /**
     * Start the worker group with a number of given
     * threads.
     *
     * @param threadSize the amount of thread
     */
    void start(size_t threadSize);

    /**
     * Join every worker, will wait for every worker
     * completation work.
     */
    void joinAll();

    /**
     * Push work into the pool.
     *
     * @param task the completation handler
     */
    void push(std::shared_ptr<Task> task);
private:
    boost::asio::io_service service_;
    std::unique_ptr<boost::asio::io_service::work> work_;
    std::vector<std::unique_ptr<TaskWorker>> workers_;
};

}; // namespace Ghrum

#endif // _TASK_WORKER_GROUP_HPP_
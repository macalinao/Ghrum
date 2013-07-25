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
#ifndef _TASK_WORKER_HPP_
#define _TASK_WORKER_HPP_

#include <Utilities/Delegate.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace Ghrum {

/**
 * Define a single work unit called Thread.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class TaskWorker {
public:
    /**
     * Default constructor of the worker.
     *
     * @param service the service
     */
    TaskWorker(boost::asio::io_service * service);

    /**
     * Destructor of the worker.
     */
    ~TaskWorker();

    /**
     * Returns if the worker is available.
     */
    bool isAvailable();

    /**
     * Set the worker to be disposed.
     */
    void setCancelled();

    /**
     * Join the worker until the worker ends its last task.
     */
    void join();
private:
    /**
     * Called to handle the run completation handler of
     * a worker.
     */
    void run();
private:
    std::unique_ptr<boost::thread> thread_;
    boost::asio::io_service * service_;
    bool available_;
};

}; // namespace Ghrum

#endif // _TASK_WORKER_HPP_
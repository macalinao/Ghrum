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

#include <Scheduler/TaskWorker.hpp>
#include <Types.hpp>
#include <chrono>

using namespace Ghrum;

/////////////////////////////////////////////////////////////////
// {@see TaskWorker::TaskWorker} ////////////////////////////////
/////////////////////////////////////////////////////////////////
TaskWorker::TaskWorker(boost::asio::io_service * service)
    : service_(service), available_(true), thread_(
        new boost::thread(Delegate<void()>(this, &TaskWorker::run))) {
}

/////////////////////////////////////////////////////////////////
// {@see TaskWorker::~TaskWorker} ///////////////////////////////
/////////////////////////////////////////////////////////////////
TaskWorker::~TaskWorker() {
    setCancelled();
}

/////////////////////////////////////////////////////////////////
// {@see TaskWorker::isAvailable} ///////////////////////////////
/////////////////////////////////////////////////////////////////
bool TaskWorker::isAvailable() {
    return available_;
}

/////////////////////////////////////////////////////////////////
// {@see TaskWorker::setCancelled} //////////////////////////////
/////////////////////////////////////////////////////////////////
void TaskWorker::setCancelled() {
    available_ = false;
}

/////////////////////////////////////////////////////////////////
// {@see TaskWorker::run} ///////////////////////////////////////
/////////////////////////////////////////////////////////////////
void TaskWorker::run() {
    do {
        // Poll a service routine, for the io_service handler,
        // if a routine cause an error, then catch it and print
        // it to the user.
        try {
            service_->poll_one();
        } catch (std::exception & ex) {
            BOOST_LOG_TRIVIAL(warning)
                    << "[!!] Worker has trigger an exception: " << ex.what();
        } catch (boost::thread_interrupted & ex) {
            BOOST_LOG_TRIVIAL(warning)
                    << "[!!] Worker has been intrrupted";
            available_ = false;
        }
        // Sleep the worker for 10 milliseconds, so the process won't
        // show 100% cpu usage.
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    } while (available_);
}

/////////////////////////////////////////////////////////////////
// {@see TaskWorker::join} /////////////////////////////////
/////////////////////////////////////////////////////////////////
void TaskWorker::join() {
    thread_->join();
}
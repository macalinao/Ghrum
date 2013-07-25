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

#include <Scheduler/SchedulerWorkerGroup.hpp>
#include <Types.hpp>

using namespace Ghrum;

/////////////////////////////////////////////////////////////////
// {@see SchedulerWorkerGroup::SchedulerWorkerGroup} ////////////
/////////////////////////////////////////////////////////////////
SchedulerWorkerGroup::SchedulerWorkerGroup(size_t threadSize)
    : service_(), work_(
        new boost::asio::io_service::work(service_)) {
    // Create all the workers and bind it to
    // a dedicate thread.
    for (size_t i = 0; i < threadSize; i++)
        workers_.push_back(std::unique_ptr<SchedulerWorker>(
                               new SchedulerWorker(&service_)));
}

/////////////////////////////////////////////////////////////////
// {@see SchedulerWorkerGroup::~SchedulerWorkerGroup} ///////////
/////////////////////////////////////////////////////////////////
SchedulerWorkerGroup::~SchedulerWorkerGroup() {
    joinAll();
}

/////////////////////////////////////////////////////////////////
// {@see SchedulerWorkerGroup::joinAll} /////////////////////////
/////////////////////////////////////////////////////////////////
void SchedulerWorkerGroup::joinAll() {
    for (size_t i = 0; i < workers_.size(); i++) {
        workers_[i]->setCancelled();
    }
    for (size_t i = 0; i < workers_.size(); i++) {
        workers_[i]->join();
    }
    work_.reset();
    service_.run();
}
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

#include <Event/EventHandler.hpp>
#include <algorithm>

using namespace Ghrum;

/////////////////////////////////////////////////////////////////
// {@see EventHandler::isEmpty} /////////////////////////////////
/////////////////////////////////////////////////////////////////
bool EventHandler::isEmpty() {
    for (int i = 0; i <= EventPriority::Monitor; i++) {
        if (delegates_[i].size() > 0)
            return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////
// {@see EventHandler::callEvent} ///////////////////////////////
/////////////////////////////////////////////////////////////////
void EventHandler::callEvent(Event & event) {
    for (int i = 0; i <= EventPriority::Monitor; i++)
        for (auto & delegate : delegates_[i])
            delegate(event);
}

/////////////////////////////////////////////////////////////////
// {@see EventHandler::addDelegate} /////////////////////////////
/////////////////////////////////////////////////////////////////
bool EventHandler::addDelegate(IEventManager::EventDelegate function, EventPriority priority) {
    std::vector<IEventManager::EventDelegate>::iterator it
        = std::find(delegates_[priority].begin(), delegates_[priority].end(), function);
    if (it != delegates_[priority].end()) {
        return false;
    }
    delegates_[priority].push_back(function);
    return true;
}

/////////////////////////////////////////////////////////////////
// {@see EventHandler::removeDelegate} //////////////////////////
/////////////////////////////////////////////////////////////////
bool EventHandler::removeDelegate(IEventManager::EventDelegate function, EventPriority priority) {
    std::vector<IEventManager::EventDelegate>::iterator it
        = std::find(delegates_[priority].begin(), delegates_[priority].end(), function);
    if (it == delegates_[priority].end()) {
        return false;
    }
    delegates_[priority].erase(it);
    return true;
}
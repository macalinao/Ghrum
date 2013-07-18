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

#include <Event/EventManager.hpp>
#include <Core/IEngineSingleton.hpp>
#include <Scheduler/TaskManager.hpp>

using namespace Ghrum;

Event & EventManager::emitEvent(Event & event, size_t id) {
    std::unordered_map<size_t, std::shared_ptr<EventHandler>>::iterator it
            = handler_.find(id);
    if (it != handler_.end()) {
        it->second->callEvent(event);
    }
    return event;
}

void EventManager::emitEventAsync(Event & event, size_t id) {
    auto delegate(Delegate<void ()>([&]() {
        emitEvent(event, id);
    }));
    IEngineSingleton::getTaskManager().asyncTask(delegate);
}

void EventManager::remove(IPlugin & owner) {
    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(accessMutexList_);
    // =================== Lock ===================

    if (plugin_.count(owner.getId()) == 0) {
        return;
    }
    std::vector<ListTuple> tuples = plugin_[owner.getId()];

    // Remove the delegates of the plugin
    for (ListTuple & tuple : tuples) {
        std::shared_ptr<EventHandler> handler = handler_[std::get<2>(tuple)];
        handler->removeDelegate( std::get<0>(tuple), std::get<1>(tuple) );
    }
    plugin_.erase(owner.getId());
}

void EventManager::removeAll() {
    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(accessMutexList_);
    // =================== Lock ===================

    plugin_.clear();
    handler_.clear();
}

bool EventManager::addListener(IPlugin & owner, EventListener & listener) {
    std::vector<EventListener::Callback> callbacks = listener.getCallback();
    for (EventListener::Callback & callback : callbacks ) {
        addDelegate(owner, *std::get<1>(callback), std::get<0>(callback),
                    std::get<2>(callback));
    }
    return true;
}

bool EventManager::removeListener(IPlugin & owner, EventListener & listener) {
    std::vector<EventListener::Callback> callbacks = listener.getCallback();
    for (EventListener::Callback & callback : callbacks ) {
        removeDelegate(owner, *std::get<1>(callback), std::get<0>(callback),
                       std::get<2>(callback));
    }
    return true;
}

bool EventManager::addDelegate(IPlugin & owner, IEventManager::Function & callback,
                               EventPriority priority, size_t id) {
    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(accessMutexList_);
    // =================== Lock ===================

    std::shared_ptr<EventHandler> & handler = handler_[id];
    if (!handler) {
        handler = std::shared_ptr<EventHandler>(new EventHandler());
    }
    bool isHandled = handler->addDelegate(callback, priority);
    if (isHandled) {
        plugin_[owner.getId()].push_back(std::make_tuple(callback, priority, id));
    }
    return isHandled;
}

bool EventManager::removeDelegate(IPlugin & owner, IEventManager::Function & callback,
                                  EventPriority priority, size_t id) {
    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(accessMutexList_);
    // =================== Lock ===================

    std::unordered_map<size_t, std::shared_ptr<EventHandler>>::iterator it
            = handler_.find(id);
    if (it == handler_.end()) {
        return false;
    }
    std::shared_ptr<EventHandler> handler = it->second;
    if (handler == nullptr) {
        return false;
    }
    bool isRemoved = handler->removeDelegate(callback, priority);
    if (isRemoved && handler->isEmpty()) {
        handler_.erase(id);
    }
    return true;
}
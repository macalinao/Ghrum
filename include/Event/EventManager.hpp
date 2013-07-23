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
#ifndef _EVENT_MANAGER_HPP_
#define _EVENT_MANAGER_HPP_

#include "EventHandler.hpp"

namespace Ghrum {

/**
 * Implementation of {@see IEventManager}.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class EventManager : public IEventManager {
public:
    /**
     * {@inheritDoc}
     */
    bool addListener(IPlugin & owner, EventListener & listener);

    /**
     * {@inheritDoc}
     */
    bool removeListener(IPlugin & owner, EventListener & listener);

    /**
     * {@inheritDoc}
     */
    void remove(IPlugin & owner);

    /**
     * {@inheritDoc}
     */
    void removeAll();
private:
    /**
     * {@inheritDoc}
     */
    void emitEvent(Event & event, size_t id);

    /**
     * {@inheritDoc}
     */
    void emitEventAsync(Event & event, size_t id);

    /**
     * {@inheritDoc}
     */
    void emitEventAsync(Event & event, Function function, size_t id);

    /**
     * {@inheritDoc}
     */
    bool addDelegate(IPlugin & owner, IEventManager::Function & callback, EventPriority priority, size_t id);

    /**
     * {@inheritDoc}
     */
    bool removeDelegate(IPlugin & owner, IEventManager::Function & callback, EventPriority priority, size_t id);
private:
    /**
     * A type definition of a tuple that the manager use.
     */
    typedef std::tuple<IEventManager::Function, EventPriority, size_t> ListTuple;
protected:
    boost::mutex accessMutexList_;
    std::unordered_map<size_t, std::vector<ListTuple>> plugin_;
    std::unordered_map<size_t, std::unique_ptr<EventHandler>> handler_;
};

}; // namespace Ghrum

#endif // _EVENT_MANAGER_HPP_
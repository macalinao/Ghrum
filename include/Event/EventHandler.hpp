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
#ifndef _EVENT_HANDLER_HPP_
#define _EVENT_HANDLER_HPP_

#include <Event/IEventManager.hpp>

namespace Ghrum {

/**
 * Encapsulate a delegate prioritized list.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class EventHandler {
public:
    /**
     * Return if the handler is empty.
     */
    bool isEmpty();

    /**
     * Emit an event into all delegates.
     *
     * @param event the event to push
     */
    void callEvent(Event & event);

    /**
     * Connect a new delegate into the handler.
     *
     * @param function pointer to the delegate function
     * @param priority priority of the delegate
     * @return true if the delegate was added succesfull
     */
    bool addDelegate(IEventManager::EventDelegate function, EventPriority priority);

    /**
     * Removes a delegate from the handler.
     *
     * @param function pointer to the delegate function
     * @param priority priority of the delegate
     * @return true if the delegate was removed succesfull
     */
    bool removeDelegate(IEventManager::EventDelegate function, EventPriority priority);
private:
    std::vector<IEventManager::EventDelegate> delegates_[EventPriority::Monitor + 1];
};

}; // namespace Ghrum

#endif // _EVENT_HANDLER_HPP_
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
#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_

#include <Core/IEngine.hpp>
#include "../Event/EventManager.hpp"
#include "../Plugin/PluginManager.hpp"
#include "../Scheduler/Scheduler.hpp"

namespace Ghrum {

/**
 * Implementation of {@see IEngine}.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class Engine : public IEngine {
public:
    /**
     * {@inheritDoc}
     */
    std::string getName();

    /**
     * {@inheritDoc}
     */
    std::string getVersion();

    /**
     * {@inheritDoc}
     */
    IPluginManager & getPluginManager();

    /**
     * {@inheritDoc}
     */
    Platform getPlatform();

    /**
     * {@inheritDoc}
     */
    IEventManager & getEventManager();

    /**
     * {@inheritDoc}
     */
    IScheduler & getScheduler();

    /**
     * Start the engine
     */
    virtual void start();

    /**
     * Stops the engine
     */
    virtual void stop();
protected:
    Scheduler scheduler_;
    PluginManager pluginManager_;
    EventManager eventManager_;
    Platform platform_;
};

}; // namespace Ghrum

#endif // _ENGINE_HPP_
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
#ifndef _GHRUM_ENGINE_HPP_
#define _GHRUM_ENGINE_HPP_

#include "Plugin/PluginManager.hpp"
#include "Event/EventManager.hpp"
#include "Scheduler/Scheduler.hpp"
#include <IEngine.hpp>

namespace Ghrum {

/**
 * Current platform name of the engine.
 */
#define PLATFORM_NAME "Ghrum"

/**
 * Current version of the engine.
 */
#define PLATFORM_VERSION "0.001.0000"

/**
 * Author of the engine.
 */
#define PLATFORM_AUTHOR "Ghrum Inc"

/**
 * Implementation of {@see IEngine}.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class GhrumEngine : public IEngine {
public:
    /**
     * Initialize the engine.
     */
    void initialize();

    /**
     * Dispose the engine.
     */
    void dispose();

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
    std::string getAuthor();

    /**
     * {@inheritDoc}
     */
    IPluginManager & getPluginManager();

    /**
     * {@inheritDoc}
     */
    IEventManager & getEventManager();

    /**
     * {@inheritDoc}
     */
    IScheduler & getScheduler();
protected:
    std::unique_ptr<PluginManager> pluginManager_;
    std::unique_ptr<EventManager> eventManager_;
    std::unique_ptr<Scheduler> scheduler_;
};

} // namespace Ghrum

#endif // _GHRUM_ENGINE_HPP_
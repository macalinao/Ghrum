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

#include <GhrumEngine.hpp>
#include <Scheduler/Scheduler.hpp>
#include <Plugin/PluginManager.hpp>
#include <Event/EventManager.hpp>

using namespace Ghrum;

/////////////////////////////////////////////////////////////////
// {@see GhrumEngine::initialize} ///////////////////////////////
/////////////////////////////////////////////////////////////////
void GhrumEngine::initialize() {
    // Build the instance of the engine.
    pluginManager_ = std::unique_ptr<PluginManager>(new PluginManager());
    eventManager_  = std::unique_ptr<EventManager>(new EventManager());
    scheduler_     = std::unique_ptr<Scheduler>(new Scheduler());

    // Find every plugin available for the current
    // platform mode.
    BOOST_LOG_TRIVIAL(info) << "Searching for plugins.";
    pluginManager_->findAndLoad();

    // Enabled those plugin that are marked to initialize before
    // the world generation, this plugins are named as core plugins.
    BOOST_LOG_TRIVIAL(info) << "Enabling OnInitialize plugins.";
    pluginManager_->enableAll(PluginOrder::OnInitialize);

    // Generate and load the current world.
    // <TODO: World Generation>

    // Enable those plugins that are marked to initialize after
    // the world generation.
    BOOST_LOG_TRIVIAL(info) << "Enabling OnWorld plugins.";
    pluginManager_->enableAll(PluginOrder::OnWorld);
}

/////////////////////////////////////////////////////////////////
// {@see GhrumEngine::dispose} //////////////////////////////////
/////////////////////////////////////////////////////////////////
void GhrumEngine::dispose() {
    // Unload every plugin loaded by the platform, and each
    // event and task allocated by them.
    pluginManager_->unloadAll();
}

/////////////////////////////////////////////////////////////////
// {@see GhrumEngine::getName} //////////////////////////////////
/////////////////////////////////////////////////////////////////
std::string GhrumEngine::getName() {
    return PLATFORM_NAME;
}

/////////////////////////////////////////////////////////////////
// {@see GhrumEngine::getVersion} ///////////////////////////////
/////////////////////////////////////////////////////////////////
std::string GhrumEngine::getVersion() {
    return PLATFORM_VERSION;
}

/////////////////////////////////////////////////////////////////
// {@see GhrumEngine::getAuthor} ////////////////////////////////
/////////////////////////////////////////////////////////////////
std::string GhrumEngine::getAuthor() {
    return PLATFORM_AUTHOR;
}

/////////////////////////////////////////////////////////////////
// {@see GhrumEngine::getPluginManager} /////////////////////////
/////////////////////////////////////////////////////////////////
IPluginManager & GhrumEngine::getPluginManager() {
    return *pluginManager_;
}

/////////////////////////////////////////////////////////////////
// {@see GhrumEngine::getEventManager} //////////////////////////
/////////////////////////////////////////////////////////////////
IEventManager & GhrumEngine::getEventManager() {
    return *eventManager_;
}

/////////////////////////////////////////////////////////////////
// {@see GhrumEngine::getScheduler} /////////////////////////////
/////////////////////////////////////////////////////////////////
IScheduler & GhrumEngine::getScheduler() {
    return *scheduler_;
}
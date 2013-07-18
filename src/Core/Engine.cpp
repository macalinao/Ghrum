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

#include <Core/Engine.hpp>
#include <Core/IEngineSingleton.hpp>
#include <thread>

using namespace Ghrum;

std::string Engine::getName() {
    return "Ghrum";
}

std::string Engine::getVersion() {
    return "0.0100.0";
}

IPluginManager & Engine::getPluginManager() {
    return pluginManager_;
}

Platform Engine::getPlatform() {
    return platform_;
}

IEventManager & Engine::getEventManager() {
    return eventManager_;
}

IScheduler & Engine::getScheduler() {
    return scheduler_;
}

void Engine::start() {
    // Sets the instance of the engine.
    IEngineSingleton::setEngine(this);

    // Find every plugin available for the current
    // platform mode, and then enable those that are
    // generated before the world.
    pluginManager_.findAvailable();
    pluginManager_.enableAll(PluginOrder::OnInitialize);

    // Generate and load the current world, and enable those
    // plugin that are marked after the world.
    // <TODO: World Generation>
    pluginManager_.enableAll(PluginOrder::OnWorld);
}

void Engine::stop() {
    // Unload every plugin loaded by the platform, and each
    // event and task allocated by them.
    pluginManager_.unloadAll();
}
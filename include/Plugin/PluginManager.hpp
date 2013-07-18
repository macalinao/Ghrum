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
#ifndef _PLUGIN_MANAGER_HPP_
#define _PLUGIN_MANAGER_HPP_

#include "Plugin.hpp"
#include <Plugin/IPluginManager.hpp>

namespace Ghrum {

/**
 * Implementation of {@see IPluginManager}.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class PluginManager : public IPluginManager {
public:
    /**
     * Default constructor.
     */
    PluginManager();

    /**
     * {@inheritDoc}
     */
    IPlugin * getPlugin(const std::string & name);

    /**
     * {@inheritDoc}
     */
    bool load(const std::string & name);

    /**
     * {@inheritDoc}
     */
    void unload(IPlugin & plugin);

    /**
     * Unload all plugins.
     */
    void unloadAll();

    /**
     * {@inheritDoc}
     */
    void enable(IPlugin & plugin);

    /**
     * Enable all plugin that is marked as loaded.
     *
     * @param order the order of which the plugins are loaded.
     */
    void enableAll(PluginOrder order);

    /**
     * {@inheritDoc}
     */
    void disable(IPlugin & plugin);

    /**
     * Disable all plugins.
     */
    void disableAll();

    /**
     * {@inheritDoc}
     */
    std::vector<IPlugin *> getPlugins();

    /**
     * Find every plugin available for loading given the
     * current order to load.
     */
    void findAvailable();
protected:
    std::string folder_;
    std::mutex accessMutexList_;
    std::unordered_map<std::string, std::shared_ptr<Plugin>> plugins_;
};

}; // namespace Ghrum

#endif // _PLUGIN_MANAGER_HPP_
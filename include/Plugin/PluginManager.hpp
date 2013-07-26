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
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/detail/xml_parser_error.hpp>

namespace Ghrum {

/**
 * Implementation of {@see IPluginManager}.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class PluginManager : public IPluginManager {
public:
    /**
     * Default constructor of the plugin manager.
     */
    PluginManager();

    /**
     * Parse the plugin descriptor, given the property file.
     *
     * @param tree the tree to parse from.
     * @param descriptor the descriptor to populate.
     */
    void getFileDescriptor(boost::property_tree::ptree & tree, PluginDescriptor & descriptor);

    /**
     * Find and load every plugin available in the
     * plugin folder.
     */
    size_t findAndLoad();

    /**
     * {@inheritDoc}
     */
    bool exist(const std::string & name);

    /**
     * {@inheritDoc}
     */
    bool load(const std::string & name);

    /**
     * {@inheritDoc}
     */
    void unload(IPlugin & plugin);

    /**
     * {@inheritDoc}
     */
    void unloadAll();

    /**
     * {@inheritDoc}
     */
    void enable(IPlugin & plugin);

    /**
     * {@inheritDoc}
     */
    void enableAll(PluginOrder order);

    /**
     * {@inheritDoc}
     */
    void disable(IPlugin & plugin);

    /**
     * {@inheritDoc}
     */
    void disableAll();

    /**
     * {@inheritDoc}
     */
    void reload(IPlugin & plugin);

    /**
     * {@inheritDoc}
     */
    void reloadAll();

    /**
     * {@inheritDoc}
     */
    IPlugin & getPlugin(const std::string & name);

    /**
     * {@inheritDoc}
     */
    std::vector<IPlugin const *> getPlugins();
protected:
    boost::mutex mutex_;
    std::string folder_;
    std::unordered_map<std::string, std::unique_ptr<Plugin>> plugins_;
};

}; // namespace Ghrum

#endif // _PLUGIN_MANAGER_HPP_
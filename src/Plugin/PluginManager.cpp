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

#include <Plugin/PluginManager.hpp>
#include <Plugin/PluginAssembly.hpp>
#include <Plugin/PluginException.hpp>
#include <Core/IEngineSingleton.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/detail/xml_parser_error.hpp>

using namespace Ghrum;


PluginManager::PluginManager()
    : folder_("." + std::string(PATH_SEPARATOR) + "Plugin" + PATH_SEPARATOR) {
}

IPlugin * PluginManager::getPlugin(const std::string & name) {
    std::unordered_map<std::string, std::shared_ptr<Plugin>>::iterator it
            = plugins_.find(name);
    return (it == plugins_.end() ? nullptr : it->second.get());
}

bool PluginManager::load(const std::string & name) {
    if ( plugins_.count(name) ) {
        return true;
    }

    // Gets the folder of the plugin, the folder
    // contains every related to the plugin.
    std::string folder(folder_ + name + PATH_SEPARATOR);

    // Loads the descriptor from the file.
    PluginDescriptor descriptor;
    try {
        boost::property_tree::ptree tree;
        read_xml(folder + PLUGIN_DESCRIPTOR_FILENAME, tree);
        descriptor.populate<boost::property_tree::ptree>(tree);
    } catch (boost::property_tree::xml_parser::xml_parser_error & ex) {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        return false;
    }

    // Check if we need to load it.
    if ( descriptor.getPlatform() != Platform::Both
            && descriptor.getPlatform() != IEngineSingleton::getPlatform() ) {
        return true;
    }

    // Inject every depencency the plugin has first.
    for (auto & dependency : descriptor.getDependencies()) {
        if ( !load(dependency) )
            return false;
    }
    for (auto & dependency : descriptor.getSoftDependencies()) {
        load(dependency);
    }

    // Create the given type of plugin and mark it for loading.
    std::shared_ptr<Plugin> plugin;
    try {
        if ( descriptor.getType() == PluginType::Assembly ) {
            plugin = std::make_shared<Plugin>(folder, descriptor);
        }
        plugin->onLoad();
    } catch (PluginException & ex) {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        return false;
    }

    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(accessMutexList_);
    // =================== Lock ===================
    plugins_.insert(
        std::pair<std::string, std::shared_ptr<Plugin>>(name, plugin));
    return true;
}

void PluginManager::enableAll(PluginOrder order) {
    for (auto & plugin : plugins_) {
        Plugin * refPlugin = plugin.second.get();

        if ( !refPlugin->isLoaded() && refPlugin->getDescriptor().getOrder() == order)
            enable(*refPlugin);
    }
    // After this point every valid plugin loaded with the given
    // order will be enabled.
}

void PluginManager::unload(IPlugin & plugin) {
    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(accessMutexList_);
    // =================== Lock ===================

    IEngineSingleton::getTaskManager().cancel(plugin);
    IEngineSingleton::getEventManager().remove(plugin);
    plugins_.erase(plugin.getName());
}

void PluginManager::unloadAll() {
    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(accessMutexList_);
    // =================== Lock ===================

    for (auto & plugin : plugins_) {
        Plugin * refPlugin = plugin.second.get();
        IEngineSingleton::getTaskManager().cancel(*refPlugin);
        IEngineSingleton::getEventManager().remove(*refPlugin);
    }
    plugins_.clear();
}

void PluginManager::enable(IPlugin & plugin) {
    if ( plugin.isEnabled() || !plugin.isLoaded() ) {
        return;
    }
    static_cast<Plugin *>(&plugin)->onEnable();
}

void PluginManager::disable(IPlugin & plugin) {
    if ( !plugin.isEnabled() || !plugin.isLoaded() ) {
        return;
    }
    static_cast<Plugin *>(&plugin)->onDisable();
}

void PluginManager::disableAll() {
    for (auto & it : plugins_)
        disable( *it.second.get() );
}

std::vector<IPlugin *> PluginManager::getPlugins() {
    std::vector<IPlugin *> list;
    for (auto & it : plugins_) {
        list.push_back( it.second.get() );
    }
    return list;
}

void PluginManager::findAvailable() {
    // Convert std::string to boost::path and create
    // the folder if doesn't exist.
    boost::filesystem::path folder(folder_);
    if ( !boost::filesystem::exists(folder) ) {
        boost::filesystem::create_directory(folder);
    }

    // Iterate over each directory and file inside plugin's folder.
    for (boost::filesystem::directory_iterator dir(folder), end;
            dir != end; dir++) {
        // Get the path to the target location.
        boost::filesystem::path entry(*dir);

        // Files and folder not valid.
        if (!boost::filesystem::is_directory(entry)
                || !boost::filesystem::exists(entry / PLUGIN_DESCRIPTOR_FILENAME)) {
            continue;
        }

        // Load the plugin.
        std::string name = entry.leaf().string();
        BOOST_LOG_TRIVIAL(info) << "Found plugin with name: " << name;

        // Try to load the plugin from the file.
        if ( !load(name) )
            BOOST_LOG_TRIVIAL(error) << "Trying to load the plugin: " << name;
    }
    // After this point every valid plugin from the plugin folder will
    // be marked for loading at the next tick of the order matchs with
    // the current plugin order member.
}
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
#include <boost/filesystem.hpp>
#include <GhrumAPI.hpp>

using namespace Ghrum;

/////////////////////////////////////////////////////////////////
// {@see PluginManager::PluginManager} //////////////////////////
/////////////////////////////////////////////////////////////////
PluginManager::PluginManager()
    : folder_(PLUGIN_FOLDER) {
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::getFileDescriptor} //////////////////////
/////////////////////////////////////////////////////////////////
void PluginManager::getFileDescriptor(boost::property_tree::ptree & tree,  PluginDescriptor & descriptor) {
    descriptor.Name = tree.get("Name", DEFAULT_NAME);
    descriptor.Website = tree.get("Website", DEFAULT_WEBSITE);
    descriptor.Version = tree.get("Version", DEFAULT_VERSION);
    descriptor.Platform = _Platform[tree.get("Platform", DEFAULT_PLATFORM)];
    descriptor.Order = _PluginOrder[tree.get("Order", DEFAULT_ORDER)];
    descriptor.Type = _PluginType[tree.get("Type", DEFAULT_TYPE)];

    // List of authors (Optional)
    if (tree.count("Authors") > 0)
        for (auto & author : tree.get_child("Authors"))
            descriptor.Authors.push_back( author.second.data() );
    // List of dependencies (Optional)
    if (tree.count("Dependencies") > 0)
        for (auto & dependency : tree.get_child("Dependencies"))
            descriptor.Dependencies.push_back( dependency.second.data() );
    // List of soft dependencies (Optional)
    if (tree.count("SoftDependencies") > 0)
        for (auto & dependency : tree.get_child("SoftDependencies"))
            descriptor.SoftDependencies.push_back( dependency.second.data() );
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::findAndLoad} ////////////////////////////
/////////////////////////////////////////////////////////////////
size_t PluginManager::findAndLoad() {
    size_t index = 0;
    // Convert std::string to boost::path and create
    // the folder if doesn't exist.
    boost::filesystem::path folder(folder_);
    if (!boost::filesystem::exists(folder)) {
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
        if (!load(name))
            BOOST_LOG_TRIVIAL(error) << "Trying to load the plugin: " << name;
        else
            index++;
    }
    return index;
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::exist} //////////////////////////////////
/////////////////////////////////////////////////////////////////
bool PluginManager::exist(const std::string & name) {
    return plugins_.count(name) > 0;
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::load} ///////////////////////////////////
/////////////////////////////////////////////////////////////////
bool PluginManager::load(const std::string & name) {
    // Don't try to load a plugin that is already
    // loaded on memory.
    if (plugins_.count(name) > 0) {
        return true;
    }

    // Gets the folder of the plugin, the folder
    // contains every related to the plugin.
    std::string folder(folder_ + name + PATH_SEPARATOR);

    // Loads the descriptor from the file and populate it
    // with important information regarding the plugin.
    PluginDescriptor descriptor;
    descriptor.Folder
        = folder;
    descriptor.Identifier
        = std::hash<std::string>()(name);
    try {
        boost::property_tree::ptree tree;
        read_xml(folder + PLUGIN_DESCRIPTOR_FILENAME, tree);
        getFileDescriptor(tree, descriptor);
    } catch (boost::property_tree::xml_parser::xml_parser_error & ex) {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        return false;
    }

    // Check if we need to load it.
    if (descriptor.Platform != Platform::Both
            && descriptor.Platform != GhrumAPI::getPlatform()) {
        return true;
    }

    // Inject every depencency the plugin has first.
    for (auto & dependency : descriptor.Dependencies) {
        if (!load(dependency))
            return false;
    }
    for (auto & dependency : descriptor.SoftDependencies) {
        load(dependency);
    }

    // Create the given type of plugin and mark it for loading.
    std::unique_ptr<Plugin> plugin = nullptr;
    try {
        if (descriptor.Type == PluginType::Assembly) {
            plugin = std::unique_ptr<Plugin>(
                         new PluginAssembly(folder, descriptor));
        } else if (descriptor.Type == PluginType::Javascript) {
            BOOST_LOG_TRIVIAL(error)
                    << "Javascript is not supported yet.";
            return false;
            // < ==================> TODO: Javascript Plugin <========================== >
            // < ==================> TODO: Javascript Plugin <========================== >
            // < ==================> TODO: Javascript Plugin <========================== >
        }
        plugin->onLoad();
    } catch (PluginException & ex) {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        return false;
    }

    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(mutex_);
    // =================== Lock ===================
    plugins_.insert(
        std::pair<std::string, std::unique_ptr<Plugin>>(name, std::move(plugin)));
    return true;
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::enableAll} //////////////////////////////
/////////////////////////////////////////////////////////////////
void PluginManager::enableAll(PluginOrder order) {
    for (auto & plugin : plugins_) {
        const std::unique_ptr<Plugin> & refPlugin = plugin.second;

        if (!refPlugin->isEnabled() && refPlugin->getDescriptor().Order == order)
            refPlugin->onEnable();
    }
    // After this point every valid plugin loaded with the given
    // order will be enabled.
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::unload} /////////////////////////////////
/////////////////////////////////////////////////////////////////
void PluginManager::unload(IPlugin & plugin) {
    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(mutex_);
    // =================== Lock ===================

    GhrumAPI::getScheduler().cancel(plugin);
    GhrumAPI::getEventManager().remove(plugin);
    plugins_.erase(plugin.getName());
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::unloadAll} //////////////////////////////
/////////////////////////////////////////////////////////////////
void PluginManager::unloadAll() {
    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(mutex_);
    // =================== Lock ===================

    for (auto & plugin : plugins_) {
        IPlugin & refPlugin = *plugin.second;
        GhrumAPI::getScheduler().cancel(refPlugin);
        GhrumAPI::getEventManager().remove(refPlugin);
    }
    plugins_.clear();
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::enable} /////////////////////////////////
/////////////////////////////////////////////////////////////////
void PluginManager::enable(IPlugin & plugin) {
    if (plugin.isEnabled())
        return;
    static_cast<Plugin &>(plugin).onEnable();
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::disable} ////////////////////////////////
/////////////////////////////////////////////////////////////////
void PluginManager::disable(IPlugin & plugin) {
    if (!plugin.isEnabled())
        return;
    static_cast<Plugin &>(plugin).onDisable();
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::disableAll} /////////////////////////////
/////////////////////////////////////////////////////////////////
void PluginManager::disableAll() {
    for (auto & plugin : plugins_)
        if (!plugin.second->isEnabled())
            plugin.second->onDisable();
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::reload} /////////////////////////////////
/////////////////////////////////////////////////////////////////
void PluginManager::reload(IPlugin & plugin) {
    // =================== Lock ===================
    std::lock_guard<std::mutex> lock(mutex_);
    // =================== Lock ===================

    // Check if the plugin can be reloaded.
    Plugin & refPlugin = static_cast<Plugin &>(plugin);
    if (!refPlugin.isReloadAllowed()) {
        return;
    }

    // Unload the plugin memory and all objects
    // registered in the platform.
    GhrumAPI::getScheduler().cancel(plugin);
    GhrumAPI::getEventManager().remove(plugin);
    refPlugin.onUnload();

    // Now load the plugin again into the memory.
    bool isLoaded = false;
    try {
        // Call to handle the plugin onLoad again.
        refPlugin.onLoad();

        // Set the plugin to be loaded back.
        isLoaded = true;
    } catch (PluginException & ex) {
        BOOST_LOG_TRIVIAL(error) << ex.what();
    }

    // if the plugin wasn't reloaded succesfull, then
    // remove it from the list of plugins, otherwise
    // enable it again.
    if (!isLoaded)
        plugins_.erase(plugin.getName());
    else
        refPlugin.onEnable();
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::reloadAll} //////////////////////////////
/////////////////////////////////////////////////////////////////
void PluginManager::reloadAll() {
    for (auto & plugin : plugins_)
        reload(*plugin.second);
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::getPlugin} //////////////////////////////
/////////////////////////////////////////////////////////////////
IPlugin & PluginManager::getPlugin(const std::string & name) {
    std::unordered_map<std::string, std::unique_ptr<Plugin>>::iterator it
            = plugins_.find(name);
    if (it == plugins_.end()) {
        throw new PluginException("Can't query the plugin given.");
    }
    return *it->second;
}

/////////////////////////////////////////////////////////////////
// {@see PluginManager::getPlugins} /////////////////////////////
/////////////////////////////////////////////////////////////////
std::vector<IPlugin const *> PluginManager::getPlugins() {
    std::vector<IPlugin const *> list;
    for (auto & it : plugins_) {
        list.push_back( it.second.get() );
    }
    return list;
}
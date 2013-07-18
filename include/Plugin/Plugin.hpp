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
#ifndef _PLUGIN_HPP_
#define _PLUGIN_HPP_

#include <Plugin/IPlugin.hpp>

namespace Ghrum {

/**
 * Implementation of {@see IPlugin}.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class Plugin : public IPlugin {
public:
    /**
     * Default constructor.
     *
     * @param folder the folder
     * @param descriptor the descriptor
     */
    Plugin(std::string & folder, PluginDescriptor & descriptor)
        : folder_(folder), descriptor_(descriptor), isLoaded_(false), isEnabled_(true) {
        id_ = std::hash<std::string>()(descriptor_.getName());
    }

    /**
     * Default destructor.
     */
    virtual ~Plugin() {
    }

    /**
     * {@inheritDoc}
     */
    PluginDescriptor & getDescriptor() {
        return descriptor_;
    }

    /**
     * {@inheritDoc}
     */
    std::string & getFolder() {
        return folder_;
    }

    /**
     * {@inheritDoc}
     */
    std::string & getName() {
        return descriptor_.getName();
    }

    /**
     * {@inheritDoc}
     */
    size_t getId() {
        return id_;
    }

    /**
     * {@inheritDoc}
     */
    bool isEnabled() {
        return isEnabled_;
    }

    /**
     * {@inheritDoc}
     */
    bool isLoaded() {
        return isLoaded_;
    }

    /**
     * {@inheritDoc}
     */
    void print(const std::string & text) {
        BOOST_LOG_TRIVIAL(info) << "<" << getName() << ">: " << text;
    }

    /**
     * {@inheritDoc}
     */
    void printError(const std::string & text) {
        BOOST_LOG_TRIVIAL(error) << "<" << getName() << ">: " << text;
    }
public:
    /**
     * Default callback to handle when the plugin has been
     * loaded.
     */
    virtual void onLoad() {
    }

    /**
     * Default callback to handle when the plugin has been
     * disabled.
     */
    virtual void onDisable() {
    }

    /**
     * Default callback to handle when the plugin has been
     * enabled.
     */
    virtual void onEnable() {
    }
protected:
    std::string folder_;
    PluginDescriptor descriptor_;
    bool isLoaded_, isEnabled_;
    size_t id_;
};

}; // namespace Ghrum

#endif // _PLUGIN_HPP_
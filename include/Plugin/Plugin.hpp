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
 * Common class for any plugin type.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class Plugin : public IPlugin {
public:
    /**
     * Default implementation of plugin ctor.
     *
     * @param folder the folder of the plugin
     * @param descriptor descriptor
     */
    Plugin(std::string & folder, PluginDescriptor & descriptor);

    /**
     * Virtual constructor to allow derivated class to destruct.
     */
    virtual ~Plugin() = default;

    /**
     * Default callback to handle when the plugin has been
     * loaded.
     */
    virtual void onLoad() = 0;

    /**
     * Default callback to handle when the plugin has been
     * disabled.
     */
    virtual void onDisable() = 0;

    /**
     * Default callback to handle when the plugin has been
     * enabled.
     */
    virtual void onEnable() = 0;

    /**
     * Default callback to handle when the plugin has been
     * unloaded.
     */
    virtual void onUnload() = 0;

    /**
     * Return if the plugin can be reloaded.
     */
    virtual bool isReloadAllowed() = 0;

    /**
     * {@inheritDoc}
     */
    const PluginDescriptor & getDescriptor() const;

    /**
     * {@inheritDoc}
     */
    const std::string & getFolder() const;

    /**
     * {@inheritDoc}
     */
    const std::string & getName() const;

    /**
     * {@inheritDoc}
     */
    size_t getId();

    /**
     * {@inheritDoc}
     */
    bool isEnabled();
protected:
    bool enabled_;
    std::string folder_;
    PluginDescriptor descriptor_;
};

} // namespace Ghrum

#endif // _PLUGIN_HPP_

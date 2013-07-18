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
#ifndef _PLUGIN_ASSEMBLY_HPP_
#define _PLUGIN_ASSEMBLY_HPP_

#include "Plugin.hpp"
#ifdef _WINDOWS_
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace Ghrum {

/**
 * Implementation of {@see Plugin} for assembly plugins.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class PluginAssembly : public Plugin {
private:
    /**
     * Definition for any plugin entry function.
     */
    typedef void (* Function)(IPlugin &);
public:
    /**
     * Default constructor.
     *
     * @param folder the folder
     * @param descriptor the descriptor
     */
    PluginAssembly(std::string & folder, PluginDescriptor & descriptor);

    /**
     * Default destructor.
     */
    ~PluginAssembly();
public:
    /**
     * {@inheritDoc}
     */
    void onLoad();

    /**
     * {@inheritDoc}
     */
    void onDisable();

    /**
     * {@inheritDoc}
     */
    void onEnable();
protected:
#ifdef _WINDOWS_
    Module handle_;
#else
    void * handle_;
#endif // _WINDOWS_
    Function fnLoad_, fnEnable_, fnDisable_;
};

}; // namespace Ghrum

#endif // _PLUGIN_ASSEMBLY_HPP_
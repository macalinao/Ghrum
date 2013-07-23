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

#include <Plugin/IPlugin.hpp>

#ifdef _WINDOWS_
/**/ #include <windows.h>
/**/
/**/ typedef HMODULE Module;
#else
/**/ #include <dlfcn.h>
/**/
/**/ typedef void * Module;
#endif // _WINDOWS_

namespace Ghrum {

/**
 * Implementation of {@see IPlugin} for assembly plugins.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class PluginAssembly : public IPlugin {
private:
    /**
     * Definition for any plugin entry function.
     */
    typedef void (* Function)(Ghrum::IPlugin &);
public:
    /**
     * Default constructor of the plugin class.
     *
     * @param descriptor the descriptor
     */
    PluginAssembly(PluginDescriptor & descriptor);

    /**
     * Destructor will dispose the library handler.
     */
    ~PluginAssembly();
public:
    /**
     * {@inheritDoc}
     */
    void handleOnLoad();

    /**
     * {@inheritDoc}
     */
    void handleOnDisable();

    /**
     * {@inheritDoc}
     */
    void handleOnEnable();

    /**
     * {@inheritDoc}
     */
    void handleOnUnload();
protected:
    Module handle_;
    Function fnLoad_, fnEnable_, fnDisable_;
};

}; // namespace Ghrum

#endif // _PLUGIN_ASSEMBLY_HPP_
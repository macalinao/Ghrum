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

#include <Plugin/PluginAssembly.hpp>
#include <Plugin/PluginException.hpp>
#include <GhrumAPI.hpp>

using namespace Ghrum;

/////////////////////////////////////////////////////////////////
// {@see PluginAssembly::PluginAssembly} ////////////////////////
/////////////////////////////////////////////////////////////////
PluginAssembly::PluginAssembly(std::string & folder, PluginDescriptor & descriptor)
    : Plugin(folder, descriptor), handle_(nullptr) {
}

/////////////////////////////////////////////////////////////////
// {@see PluginAssembly::~PluginAssembly} ///////////////////////
/////////////////////////////////////////////////////////////////
PluginAssembly::~PluginAssembly() {
    onUnload();
}

/////////////////////////////////////////////////////////////////
// {@see PluginAssembly::onDisable} /////////////////////////////
/////////////////////////////////////////////////////////////////
void PluginAssembly::onDisable() {
    if (fnDisable_ != nullptr)
        fnDisable_(*this);
    enabled_ = false;
}

/////////////////////////////////////////////////////////////////
// {@see PluginAssembly::onEnable} //////////////////////////////
/////////////////////////////////////////////////////////////////
void PluginAssembly::onEnable() {
    if (fnEnable_ != nullptr)
        fnEnable_(*this);
    enabled_ = true;
}

/////////////////////////////////////////////////////////////////
// {@see PluginAssembly::onLoad} ////////////////////////////////
/////////////////////////////////////////////////////////////////
void PluginAssembly::onLoad() {
    // Get the local file path, on each operative system, the extension
    // is quite different. (Windows = .dll, Unix = .so)
    std::string filename = folder_ + LIBRARY_NAME;

    // Load the shared library.
#ifdef _WINDOWS_
    handle_ = ::LoadLibrary(filename.c_str());
#else
    handle_ = dlopen(filename.c_str(), RTLD_LAZY);
#endif // _WINDOWS_
    if (handle_ == nullptr) {
        throw PluginException("The shared library cannot be loaded.");
    }

    // Gets all function from the library.
#ifdef _WINDOWS_
    fnLoad_    = (Function)GetProcAddress(handle_, "_Z12onPluginLoadRN5Ghrum7IPluginE");
    fnEnable_  = (Function)GetProcAddress(handle_, "_Z14onPluginEnableRN5Ghrum7IPluginE");
    fnDisable_ = (Function)GetProcAddress(handle_, "_Z15onPluginDisableRN5Ghrum7IPluginE");
#else
    fnLoad_    = (Function)dlsym(handle_, "_Z12onPluginLoadRN5Ghrum7IPluginE");
    fnEnable_  = (Function)dlsym(handle_, "_Z14onPluginEnableRN5Ghrum7IPluginE");
    fnDisable_ = (Function)dlsym(handle_, "_Z15onPluginDisableRN5Ghrum7IPluginE");
#endif // _WINDOWS_

    // Set the engine pointer if the plugin use it.
    typedef GhrumAPI & (*pfSingleton)();
#ifdef _WINDOWS_
    pfSingleton fnEngine
        = (pfSingleton)GetProcAddress(handle_, "_ZN5Ghrum16IEngineSingleton11getInstanceEv");
#else
    pfSingleton fnEngine
        = (pfSingleton)dlsym(handle_, "_ZN5Ghrum16IEngineSingleton11getInstanceEv");
#endif // _WINDOWS_
    if (fnEngine != nullptr) {
        fnEngine().setInstance(&GhrumAPI::getEngine());
    }

    // Call the first plugin function.
    if (fnLoad_ != nullptr)
        fnLoad_(*this);
}

/////////////////////////////////////////////////////////////////
// {@see PluginAssembly::onUnload} //////////////////////////////
/////////////////////////////////////////////////////////////////
void PluginAssembly::onUnload() {
    if (handle_ != nullptr)
#ifdef _WINDOWS_
        ::FreeLibrary(handle_);
#else
        dlclose(handle_);
#endif // _WINDOWS_
    handle_ = nullptr;
}

/////////////////////////////////////////////////////////////////
// {@see PluginAssembly::isReloadAllowed} ///////////////////////
/////////////////////////////////////////////////////////////////
bool PluginAssembly::isReloadAllowed() {
    return false;
}
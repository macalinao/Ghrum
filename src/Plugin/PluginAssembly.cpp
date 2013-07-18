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
#include <Core/IEngineSingleton.hpp>

using namespace Ghrum;

PluginAssembly::PluginAssembly(std::string & folder, PluginDescriptor & descriptor)
    : Plugin(folder, descriptor), handle_(nullptr) {
}

PluginAssembly::~PluginAssembly() {
    if (handle_ != nullptr)
#ifdef _WINDOWS_
        ::FreeLibrary(handle_);
#else
        dlclose(handle_);
#endif // _WINDOWS_
}

void PluginAssembly::onLoad() {
    if (fnDisable_ != nullptr) {
        fnDisable_(*this);
    }
    isEnabled_ = false;
}

void PluginAssembly::onDisable() {
    if (fnEnable_ != nullptr) {
        fnEnable_(*this);
    }
    isEnabled_ = true;
}

void PluginAssembly::onEnable() {
#ifdef _WINDOWS_
    handle_ = ::LoadLibrary(
                  std::string(folder_ + "libPlugin.dll").c_str());
#else
    handle_ = dlopen(
                  std::string(folder_ + "libPlugin.so").c_str(), RTLD_LAZY);
#endif // _WINDOWS_

    if (handle_ == nullptr) {
        throw PluginException("Cannot load library file.");
    }

    // Gets all function from the library.
#ifdef _WINDOWS_
    fnLoad_    = (PluginAssembly::Function)GetProcAddress(handle_, "_Z12onPluginLoadRN5Ghrum7IPluginE");
    fnEnable_  = (PluginAssembly::Function)GetProcAddress(handle_, "_Z14onPluginEnableRN5Ghrum7IPluginE");
    fnDisable_ = (PluginAssembly::Function)GetProcAddress(handle_, "_Z15onPluginDisableRN5Ghrum7IPluginE");
#else
    fnLoad_    = (PluginAssembly::Function)dlsym(handle_, "_Z12onPluginLoadRN5Ghrum7IPluginE");
    fnEnable_  = (PluginAssembly::Function)dlsym(handle_, "_Z14onPluginEnableRN5Ghrum7IPluginE");
    fnDisable_ = (PluginAssembly::Function)dlsym(handle_, "_Z15onPluginDisableRN5Ghrum7IPluginE");
#endif // _WINDOWS_

    // Set the engine pointer if the plugin use it.
    typedef IEngineSingleton & (*pfSingleton)();
#ifdef _WINDOWS_
    pfSingleton fnEngine
        = (pfSingleton)GetProcAddress(handle_, "_ZN5Ghrum16IEngineSingleton11getInstanceEv");
#else
    pfSingleton fnEngine
        = (pfSingleton)dlsym(handle_, "_ZN5Ghrum16IEngineSingleton11getInstanceEv");
#endif // _WINDOWS_
    if (fnEngine != nullptr) {
        fnEngine().setInstance(IEngineSingleton::getEngine());
    }

    // Call the first plugin function.
    if (fnLoad_ != nullptr) {
        fnLoad_(*this);
    }
    isLoaded_ = true;
}
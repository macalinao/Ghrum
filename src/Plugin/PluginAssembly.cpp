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
#include <boost/filesystem.hpp>

using namespace Ghrum;

/////////////////////////////////////////////////////////////////
// {@see PluginAssembly::PluginAssembly} ////////////////////////
/////////////////////////////////////////////////////////////////
PluginAssembly::PluginAssembly(PluginDescriptor & descriptor)
    : IPlugin(descriptor), handle_(nullptr) {
}

/////////////////////////////////////////////////////////////////
// {@see PluginAssembly::~PluginAssembly} ///////////////////////
/////////////////////////////////////////////////////////////////
PluginAssembly::~PluginAssembly() {
    handleOnUnload();
}

/////////////////////////////////////////////////////////////////
// {@see PluginAssembly::handleOnDisable} ///////////////////////
/////////////////////////////////////////////////////////////////
void PluginAssembly::handleOnDisable() {
    if (fnDisable_ != nullptr) {
        fnDisable_(*this);
    }
    enabled_ = false;
}

/////////////////////////////////////////////////////////////////
// {@see PluginAssembly::handleOnEnable} ////////////////////////
/////////////////////////////////////////////////////////////////
void PluginAssembly::handleOnEnable() {
    if (fnEnable_ != nullptr) {
        fnEnable_(*this);
    }
    enabled_ = true;
}

/////////////////////////////////////////////////////////////////
// {@see PluginAssembly::handleOnLoad} //////////////////////////
/////////////////////////////////////////////////////////////////
void PluginAssembly::handleOnLoad() {
    // Get the local file path, on each operative system, the extension
    // is quite different. (Windows = .dll, Unix = .so)
#ifdef _WINDOWS_
    std::string filename = "plugin.dll";
#else
    std::string filename = "plugin.so";
#endif

    // Copy the library file into a temp directory, to allow
    // reload of the plugin for new code inserted.
    boost::filesystem::path tmpFolder(getFolder() + ".lock");
    boost::filesystem::path tmpFilename(tmpFolder / filename);
    boost::filesystem::create_directories(tmpFolder);
    boost::filesystem::copy_file(boost::filesystem::path(getFolder() + filename),
                                 tmpFilename,
                                 boost::filesystem::copy_option::overwrite_if_exists);

    // Load the shared library.
#ifdef _WINDOWS_
    handle_ = ::LoadLibrary(tmpFilename.c_str());
#else
    handle_ = dlopen(tmpFilename.c_str(), RTLD_LAZY);
#endif // _WINDOWS_

    if (handle_ == nullptr) {
        throw PluginException("Cannot load library file.");
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
        fnEngine().setInstance(GhrumAPI::getEngine());
    }

    // Call the first plugin function.
    if (fnLoad_ != nullptr)
        fnLoad_(*this);
}

/////////////////////////////////////////////////////////////////
// {@see PluginAssembly::handleOnUnload} ////////////////////////
/////////////////////////////////////////////////////////////////
void PluginAssembly::handleOnUnload() {
    // Call the function to dispose the shared library
    // from the process's memory.
    if (handle_ != nullptr)
#ifdef _WINDOWS_
        ::FreeLibrary(handle_);
#else
        dlclose(handle_);
#endif // _WINDOWS_

    // Removes the temporally file created for the shared
    // library.
    boost::filesystem::remove_all(
        boost::filesystem::path(getFolder() + ".lock"));
}
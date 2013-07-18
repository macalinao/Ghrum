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

#include <Core/EngineServer.hpp>

using namespace Ghrum;

void EngineServer::start() {
    // The current platform is server only.
    platform_ = Platform::Server;

    // Calls the base function of the server
    // instance.
    Engine::start();
}

void EngineServer::stop() {
    // Calls the base function of the server
    // instance.
    Engine::stop();
}
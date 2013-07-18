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
#ifndef _ENGINE_SERVER_HPP_
#define _ENGINE_SERVER_HPP_

#include "Engine.hpp"
#include <Core/IEngineServer.hpp>

namespace Ghrum {

/**
 * Implementation of {@see IEngineServer}.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class EngineServer : public Engine {
public:
#ifdef _SERVER_PRODUCTION_
    /**
     * {@inheritDoc}
     */
    ISessionManager & getSessionManager() {
        return sessionManager_;
    }
#endif // _SERVER_PRODUCTION_

    /**
     * {@inheritDoc}
     */
    IPermissibleGroup & getDefaultGroup();

    /**
     * {@inheritDoc}
     */
    IPermissibleGroup * getGroup(const std::string & name);

    /**
     * {@inheritDoc}
     */
    std::vector<IPermissibleGroup *> getGroups();
public:
    /**
     * {@inheritDoc}
     */
    void start();

    /**
     * {@inheritDoc}
     */
    void stop();
private:
#ifdef _SERVER_PRODUCTION_
    SessionManager sessionManager_;
#endif // _SERVER_PRODUCTION_ 
};

}; // namespace Ghrum

#endif // _ENGINE_SERVER_HPP_
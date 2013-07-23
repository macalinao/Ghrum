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
#ifndef _SESSION_HPP_
#define _SESSION_HPP_

#include <Network/ISession.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>

namespace Ghrum {

/**
 * Implementation of {@see ISession}.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class Session : public ISession {
public:
    /**
     * Constructor of a session.
     *
     * @param service the proactor service
     * @param id the id of the session
     */
    Session(boost::asio::io_service & service, size_t id);

    /**
     * {@inheritDoc}
     */
    void disconnect(const std::string & reason);

    /**
     * {@inheritDoc}
     */
    std::string getAddress();

    /**
     * {@inheritDoc}
     */
    void setProtocol(IProtocol & protocol);

    /**
     * {@inheritDoc}
     */
    void start();

    /**
     * {@inheritDoc}
     */
    void sendMessage(IMessage & message);
protected:
    /**
     * Handle when the client connect to the server
     *
     * @param error error code of the operation
     * @param endpointIterator address where to connect
     */
    void handleConnect(const boost::system::error_code & error,
                       boost::asio::ip::tcp::resolver::iterator endpointIterator);

    /**
     * Handle when the client receives data
     *
     * @param error error code of the operation
     */
    void handleRead(const boost::system::error_code & error, std::size_t bytes_transferred);

    /**
     * Handle the serialization of a message
     *
     * @param message the message to write
     */
    void handleWriteMessage(IMessage & message);
private:
    boost::asio::ip::tcp::socket socket_;
};

}; // namespace Ghrum

#endif // _SESSION_HPP_
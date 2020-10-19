/*
 * MIT License
 *
 * Copyright (c) 2020 pengwang7
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __JSONRPC_TRANSPORT_SERVER_HPP__
#define __JSONRPC_TRANSPORT_SERVER_HPP__

#include <asio.hpp>

#include "callback.hpp"
#include "io_service_pool.hpp"
#include "connection_manager.hpp"

namespace jsonrpc {

class ServerConfiguration {
public:
    ServerConfiguration(unsigned short port = 8025) noexcept : port(port) {}

public:
    // Port number to use. Defaults to 8025 for RPC. Set to 0 get an assigned port.
    unsigned short port;

    // IPv4 address in dotted decimal form or IPv6 address in hexadecimal notation.
    // If empty, the address will be any address.
    std::string address;

    // If io_service is not set, number of threads that the server will use when start() is called.
    // Defaults to 4 thread.
    std::size_t io_service_pool_size = 4;

    // Default used http protocol for RPC.
    bool use_http_protocol = true;

    // Timeout on request handling. Defaults to 5 seconds.
    int timeout_request = 5;

    // Timeout on content handling. Defaults to 300 seconds.
    int timeout_content = 300;

    // Maximum size of request stream buffer. Defaults to architecture maximum.
    // Reaching this limit will result in a message_size error code.
    std::size_t max_streambuf_size = std::numeric_limits<std::size_t>::max();

    // Set to false to avoid binding the socket to an address that is already in use. Defaults to true.
    bool reuse_address = true;    
};

class TransportServer : private asio::noncopyable {
public:
    TransportServer(ServerConfiguration& config) noexcept;

    ~TransportServer();

public:
    void run();

    void stop();

public:
    void setConnectionCallback(const ConnectionCallback& fn) {
        conn_fn_ = fn;
    }

    void setMessageCallback(const ReadMessageCallback& fn) {
        message_fn_ = fn;
    }

    void setWriteCompleteCallback(const WriteCompleteCallback& fn) {
        write_complete_fn_ = fn;
    }

    void setTimedoutCallback(const TimedoutCallback& fn) {
        timedout_fn_ = fn;
    }

    void setClosedCallback(const ClosedCallback& fn) {
        closed_fn_ = fn;
    }

private:
    void doSocketAccept();

private:
    ServerConfiguration& config_;

    IOServicePool io_service_pool_;

    asio::ip::tcp::acceptor acceptor_;

    ConnectionManager conn_manager_;

    ConnectionCallback conn_fn_;

    ReadMessageCallback message_fn_;

    WriteCompleteCallback write_complete_fn_;

    TimedoutCallback timedout_fn_;

    ClosedCallback closed_fn_;
};

} /* end namespace jsonrpc */

#endif /* __JSONRPC_TRANSPORT_SERVER_HPP__ */

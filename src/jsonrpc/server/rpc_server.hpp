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

#ifndef __JSONRPC_SERVER_HPP__
#define __JSONRPC_SERVER_HPP__

#include "connection.hpp"
#include "transport_server.hpp"
#include "rpc_channel.hpp"

namespace jsonrpc {

class Dispatcher;

class RpcServer {
public:
    typedef std::shared_ptr<TransportServer> transport_server_ptr;
    typedef std::shared_ptr<RpcChannel> rpc_channel_ptr;

public:
    explicit RpcServer(ServerConfiguration& config)
        : config_(config), transport_server_(new TransportServer(config_)) {
        doInit();
    }

    ~RpcServer() {}

public:
    const std::shared_ptr<Dispatcher>& dispatcherInstance() const {
        return dispatcher_;
    }

    void run() {
        transport_server_->run();
    }

    void stop() {
        transport_server_->stop();
    }

    void setWriteCompleteCallback(const WriteCompleteCallback& fn) {
        transport_server_->setWriteCompleteCallback(fn);
    }

    void setTimedoutCallback(const TimedoutCallback& fn) {
        transport_server_->setTimedoutCallback(fn);
    }

    void setClosedCallback(const ClosedCallback& fn) {
        transport_server_->setClosedCallback(fn);
    }

private:
    void doInit() {
        transport_server_->setConnectionCallback(std::bind(&RpcServer::onConnection, this, std::placeholders::_1));
    }

    void onConnection(const connection_ptr& conn) {
        rpc_channel_ptr channel(new RpcChannel(conn, dispatcher_));
        transport_server_->setMessageCallback(std::bind(&RpcChannel::onRpcMessage, channel.get(), std::placeholders::_1, std::placeholders::_2));
        conn->setContext(channel);
    }

private:
    ServerConfiguration config_;

    transport_server_ptr transport_server_;

    std::shared_ptr<Dispatcher> dispatcher_;
};

} /* end namespace jsonrpc */

#endif /* __JSONRPC_SERVER_HPP__ */

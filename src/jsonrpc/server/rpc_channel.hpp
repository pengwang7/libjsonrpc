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

#ifndef __JSONRPC_CHANNEL_HPP__
#define __JSONRPC_CHANNEL_HPP__

#include "utility.hpp"
#include "callback.hpp"

#include "glog/logging.h"

namespace jsonrpc {

class Dispatcher;

class RpcChannel {
public:
    typedef std::shared_ptr<Dispatcher> dispatcher_ptr;

public:
    RpcChannel(const connection_ptr& conn, const dispatcher_ptr& dispatcher)
    : conn_(conn), dispatcher_(dispatcher) {

    }

    ~RpcChannel() {
        LOG(INFO) << "The rpc channel destroy";
    }

    void onRpcMessage(const connection_ptr& conn, std::string& message) {
        LOG(INFO) << "The rpc message: " << message;

        Json::Value table = Json::Value::null;
        if (!Codec::decode(message, table)) {
            onRpcRequest(table);
        } else {
            conn->stop(false);
        }
    }

    void onRpcRequest(Json::Value& table) {
        std::size_t id = table[RPC_ID].asInt();
        std::string version = table[RPC_VERSION].asCString();
        std::string call_method = table[RPC_METHOD].asCString();
        Json::Value call_params = table[RPC_PARAMS];

        LOG(INFO) << id;
        LOG(INFO) << version;
        LOG(INFO) << call_method;
        //LOG(INFO) << call_params;

        onRpcResponse(callMethod());
    }

    void onRpcResponse(const std::string& message) {
        Json::Value table = Json::Value::null;
        conn_->send(message);
    }

private:
    std::string callMethod() {
        // test
        return std::string("jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj\n");
    }

private:
    connection_ptr conn_;

    dispatcher_ptr dispatcher_;
};

} /* end namespace jsonrpc */

#endif /* __JSONRPC_CHANNEL_HPP__ */

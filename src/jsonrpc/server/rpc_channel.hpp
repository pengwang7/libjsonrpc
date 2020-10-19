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

	}

	void onRpcMessage(const connection_ptr& conn, std::string& message) {
        LOG(INFO) << "The rpc message: " << message;

        Json::Value table = Json::Value::null;
        if (!Codec::decode(message, table)) {
            onRpcRequest(table);
        } else {
            conn_->stop(false);
        }
	}

	void onRpcRequest(Json::Value& table) {
        onRpcResponse(callMethod());
	}

    void onRpcResponse(const std::string& message) {
        conn_->send(message);
    }

private:
	std::string callMethod() {
        // test
        return std::string("");
	}

private:
	connection_ptr conn_;

	dispatcher_ptr dispatcher_;
};

} /* end namespace jsonrpc */

#endif /* __JSONRPC_CHANNEL_HPP__ */

#ifndef __RPC_CHANNEL_HPP__
#define __RPC_CHANNEL_HPP__

#include "callback.hpp"

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

	void onMessage(const connection_ptr& conn, const char* message) {
		printf("RpcChannel on message: %s\n", message);

        conn->send(message);
	}

	void onRpcMessage() {

	}

	void onRpcRequest() {

	}

	void onRpcResponse() {

	}

private:
	void callMethod() {

	}

private:
	connection_ptr conn_;

	dispatcher_ptr dispatcher_;
};

} /* end namespace jsonrpc */

#endif /* __RPC_CHANNEL_HPP__ */

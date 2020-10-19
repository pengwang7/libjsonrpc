#ifndef __JSON_RPC_SERVER_HPP__
#define __JSON_RPC_SERVER_HPP__

#include "server.hpp"
#include "connection.hpp"
//#include "rpc_channel.hpp"

namespace jsonrpc {

#if 0
class Dispatcher;

class RpcServer {
public:
    typedef Server tcp_server;
	typedef std::shared_ptr<tcp_server> tcp_server_ptr;
    typedef std::shared_ptr<RpcChannel> rpc_channel_ptr;

public:
    explicit RpcServer(std::string service_address, unsigned int service_port,
        std::size_t io_service_pool_size, std::size_t message_protocol)
        : tcp_server_(new tcp_server(service_address, service_port, io_service_pool_size, message_protocol)),
          service_addrress_(service_address),
          service_port_(service_port),
          io_service_pool_size_(io_service_pool_size) {
        doInit();
    }

    ~RpcServer() {}

public:
    const std::shared_ptr<Dispatcher>& dispatcherInstance() const {
        return dispatcher_;
    }

    void run() {
        tcp_server_->run();
    }

    void stop() {
        tcp_server_->stop();
    }

private:
    void doInit() {
        tcp_server_->setConnectionCallback(std::bind(&RpcServer::onConnection, this, std::placeholders::_1));
    }

    void onConnection(const connection_ptr& conn) {
        printf("rpc on connection\n");
        rpc_channel_ptr channel(new RpcChannel(conn, dispatcher_));
        tcp_server_->setMessageCallback(std::bind(&RpcChannel::onMessage, channel, std::placeholders::_1, std::placeholders::_2));
    }
#if 0
    void setWriteCompleteCallback(const tcp::WriteCompleteCallback& fn) {
        tcp_server_->setWriteCompleteCallback(fn);
    }

    void setTimedoutCallback(const tcp::TimedoutCallback& fn) {
        tcp_server_->setTimedoutCallback(fn);
    }

    void setClosedCallback(const tcp::ClosedCallback& fn) {
        tcp_server_->setClosedCallback(fn);
    }

    void setSocketReadTimeoutSeconds(const unsigned int seconds) {
        tcp_server_->setConnectionReadTimeoutSeconds(seconds);
    }
private:
    void onCompleteMessage(const tcp::connection_ptr& conn, const char* complete_data) {
        message_fn_(conn, complete_data);
    }
#endif
private:
	tcp_server_ptr tcp_server_;

    std::string service_addrress_;

    unsigned int service_port_;

    std::size_t io_service_pool_size_;

    unsigned int timeout_seconds_;

    std::shared_ptr<Dispatcher> dispatcher_;
};
#endif
} /* end namespace jsonrpc */

#endif /* __JSON_RPC_SERVER_HPP__ */

#include "utility.hpp"
#include "content.hpp"
#include "io_service_pool.hpp"
#include "transport_server.hpp"
#include "connection_manager.hpp"
#include "rpc_server.hpp"

#include "glog/logging.h"

void doInitLogger() {
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr = true;
    FLAGS_logbufsecs = 0;
    FLAGS_max_log_size = 1800;

    google::InitGoogleLogging("test");
    google::SetLogDestination(google::GLOG_INFO,"log-");
}

void doCloseLogger() {

}

class test_rpc_server : public jsonrpc::RpcServer {
public:
    test_rpc_server(jsonrpc::ServerConfiguration config) : RpcServer(config) {
        setClosedCallback(std::bind(&test_rpc_server::onClose, this, std::placeholders::_1));
    }

private:
    // typedef std::function<void(const connection_ptr&)> ClosedCallback;
    void onClose(const jsonrpc::connection_ptr& conn) {
        LOG(INFO) << "on close";
    }
};

void rpc_server_example() {
    jsonrpc::ServerConfiguration config;
    config.port = 8025;
    config.use_http_protocol = true;

    test_rpc_server rpc_server(config);
    rpc_server.run();
}

int main() {
	doInitLogger();

	LOG(INFO) << "test";

    rpc_server_example();
#if 0
    asio::streambuf streambuf;
    jsonrpc::Content content(streambuf);

    LOG(INFO) << "sizeof content: " << sizeof(content);
#endif
#if 0
    jsonrpc::ServerConfiguration config;
    config.port = 8025;
    config.use_http_protocol = true;

    jsonrpc::TransportServer server(config);
    server.run();
#endif
	return 0;
}

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

int main() {
	doInitLogger();

	LOG(INFO) << "test";
#if 0
    asio::streambuf streambuf;
    jsonrpc::Content content(streambuf);

    LOG(INFO) << "sizeof content: " << sizeof(content);
#endif

    jsonrpc::ServerConfiguration config;
    config.port = 8025;
    config.use_http_protocol = true;

    jsonrpc::TransportServer server(config);
    server.run();

	return 0;
}

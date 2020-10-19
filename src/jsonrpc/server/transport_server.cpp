#include "connection.hpp"
#include "transport_server.hpp"

namespace jsonrpc {

TransportServer::TransportServer(ServerConfiguration& config) noexcept
    : config_(config), io_service_pool_(config_.io_service_pool_size),
      acceptor_(io_service_pool_.getIOService()), conn_manager_(config_.use_http_protocol) {
    // TransportServer bind and accept from server configuration.
    asio::ip::tcp::endpoint endpoint;
    if(config_.address.size() > 0) {
        endpoint = asio::ip::tcp::endpoint(asio::ip::address::from_string(config_.address), config_.port);
    } else {
        endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), config_.port);
    }
    
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    doSocketAccept();
}

TransportServer::~TransportServer() {
    conn_manager_.stopAll();
}

void TransportServer::run() {
    io_service_pool_.run();
}

void TransportServer::stop() {
    io_service_pool_.stop();
}

void TransportServer::doSocketAccept() {
    connection_ptr conn(new Connection(io_service_pool_.getIOService(), config_.max_streambuf_size, conn_manager_));
    asio::ip::tcp::socket& socket = conn->socket();

    acceptor_.async_accept(socket,
        [this, conn = std::move(conn)](const asio::error_code& ec) -> void {
            if (!acceptor_.is_open() || ec) {
                return;
            }

            if (ec != asio::error::operation_aborted) {
                doSocketAccept();
            }

            if (conn_fn_) {
                conn_fn_(conn);
            }

            conn->setReadMessageCallback(message_fn_);
            conn->setWriteCompleteCallback(write_complete_fn_);
            conn->setCloseCallback(closed_fn_);

            conn_manager_.start(conn);
        }
    );
}

} /* end namespace jsonrpc */

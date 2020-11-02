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

#include <sys/time.h>
#include <sys/resource.h>

#include "connection.hpp"
#include "transport_server.hpp"

namespace jsonrpc {

TransportServer::TransportServer(ServerConfiguration& config) noexcept
    : config_(config), io_service_pool_(config_.io_service_pool_size),
      acceptor_(io_service_pool_.getIOService()), conn_manager_(config_.use_http_protocol) {
    // Set process open max file descriptor limit.
    struct rlimit curr_limit, core_limit;
    if (!getrlimit(RLIMIT_NOFILE, &curr_limit)) {
        core_limit.rlim_cur = config_.fd_limits;
        core_limit.rlim_max = config_.fd_limits;

        if (setrlimit(RLIMIT_NOFILE, &core_limit) != 0) {
            // Try raising just to the original when failed.
            core_limit.rlim_cur = core_limit.rlim_max = curr_limit.rlim_max;
            setrlimit(RLIMIT_NOFILE, &core_limit);
        }
    }

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

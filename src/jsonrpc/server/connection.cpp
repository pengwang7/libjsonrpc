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

#include "utility.hpp"
#include "connection.hpp"
#include "connection_manager.hpp"

#include "glog/logging.h"

namespace jsonrpc {

#define APPLICATION_JSON "application/json"

Connection::Connection(asio::io_service& io_service, std::size_t max_streambuf_size, ConnectionManager& manager)
    : socket_(io_service), streambuf_(max_streambuf_size),
      steady_timer_(io_service), conn_manager_(manager) {

}

void Connection::start() {
    asio::ip::tcp::no_delay no_delay(true);
    socket_.set_option(no_delay);
    socket_.set_option(asio::detail::socket_option::boolean<IPPROTO_TCP, TCP_QUICKACK>(true));

    if (conn_manager_.protocol()) {
        asio::async_read_until(socket_, streambuf_, "\r\n\r\n",
            std::bind(&Connection::netSocketReadHandle, shared_from_this(),
            std::placeholders::_1, std::placeholders::_2));
    } else {
        asio::async_read(socket_, asio::buffer(&data_length_, sizeof(int32_t)),
            std::bind(&Connection::netSocketReadHandle, shared_from_this(),
            std::placeholders::_1, std::placeholders::_2));
    }
}

void Connection::stop(bool internal) {
    if (!socket_.is_open()) {
        return;
    }

    if (internal) {
        asio::error_code ec;
        socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        socket_.close(ec);
    } else {
        conn_manager_.stop(shared_from_this());
    }

    stopSocketReadTimedout();
}

void Connection::send(const char* data, std::size_t n) {
    send(std::string(data, n));
}

void Connection::send(std::string data) {
    std::string response_message = ResponseMessage::message(data, conn_manager_.protocol());

    netSocketWriteHandle(response_message);
}

void Connection::netSocketReadHandle(const asio::error_code& ec, std::size_t bytes_transferred) {
    stopSocketReadTimedout();

    if (ec) {
        LOG(ERROR) << "The socket read met error: " << ec.message();

        if (!bytes_transferred && closed_fn_) {
            closed_fn_(shared_from_this());
        }

        conn_manager_.stop(shared_from_this());

        return;
    }

    std::size_t bytes_additional = streambuf_.size() - bytes_transferred;

    Content content(streambuf_);
    std::unordered_map<std::string, std::string> headers;

    if (!RequestMessage::parse(content, headers, conn_manager_.protocol())) {
        conn_manager_.stop(shared_from_this());
        return;
    }

    std::string content_type = headers["content-type"];
    std::string content_length = headers["content-length"];

    data_length_ = std::stoll(content_length);

    if (content_type != APPLICATION_JSON || data_length_ <= 0) {
        LOG(ERROR) << "HTTP Content-Type or Content-Length error";
        conn_manager_.stop(shared_from_this());
        return;
    }

    if (data_length_ > bytes_additional) {

    } else {
        std::string data = content.string();

        LOG(INFO) << "The http content: " << data;

        if (read_fn_) {
            read_fn_(shared_from_this(), data);
        }

        // No need to call asio::streambuf.commit()
    }
}

void Connection::netSocketWriteHandle(const std::string& message) {
    if (!message.size()) {
        return;
    }

    SharedConstBuffer const_buffer(message);

    asio::async_write(socket_, const_buffer,
        [this, transport_length = message.size(), self = shared_from_this()](const asio::error_code& ec, std::size_t bytes_transferred) -> void {
            if (!ec) {
                if (write_complete_fn_ && (transport_length == bytes_transferred)) {
                    write_complete_fn_(self);
                }
            } else {
                conn_manager_.stop(self);
            }
        }
    );
}

void Connection::startSocketReadTimedout() {
    if (!timeout_seconds_) {
        return;
    }

    steady_timer_.expires_from_now(std::chrono::seconds(timeout_seconds_));

    steady_timer_.async_wait(
        [this, self = shared_from_this()](const asio::error_code& ec) ->void {
            if (!ec) {
                if (timedout_fn_) {
                    timedout_fn_(self);
                }
            }

            conn_manager_.stop(self);
        }
    );
}

void Connection::stopSocketReadTimedout() {
    if (!timeout_seconds_) {
        return;
    }

    asio::error_code ec;
    steady_timer_.cancel(ec);
}

} /* end namespace jsonrpc */

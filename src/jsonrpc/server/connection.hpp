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

#ifndef __JSONRPC_CONNECTION_HPP__
#define __JSONRPC_CONNECTION_HPP__

#include <asio.hpp>

#include "any.hpp"
#include "content.hpp"
#include "callback.hpp"

namespace jsonrpc {

class SharedConstBuffer {
public:
    typedef asio::const_buffer value_type;
    typedef const asio::const_buffer* const_iterator;

public:
    explicit SharedConstBuffer(const std::string& data) noexcept
        : data_(new std::vector<char>(data.begin(), data.end())),
          buffer_(asio::buffer(*data_)) {}

    ~SharedConstBuffer() {}

public:
    const asio::const_buffer* begin() const { return &buffer_; }

    const asio::const_buffer* end() const { return &buffer_ + 1; }

private:
    std::shared_ptr<std::vector<char>> data_;

    asio::const_buffer buffer_;
};

class Content;
class ConnectionManager;

class Connection : public std::enable_shared_from_this<Connection>, private asio::noncopyable {
public:
    explicit Connection(asio::io_service& io_service, std::size_t max_streambuf_size, ConnectionManager& manager);

    ~Connection();

public:
    asio::ip::tcp::socket& socket() { return socket_; }

    void start();

    void stop(bool internal = false);

    void send(const char* data, std::size_t n);

    void send(std::string data);

public:
    void setContext(const any& context) {
        context_ = context;
    }

    any& getContext() {
        return context_;
    }

    void setReadMessageCallback(const ReadMessageCallback& fn) {
        read_fn_ = fn;
    }

    void setWriteCompleteCallback(const WriteCompleteCallback& fn) {
        write_complete_fn_ = fn;
    }

    void setTimedoutCallback(const TimedoutCallback& fn) {
        timedout_fn_ = fn;
    }

    void setCloseCallback(const ClosedCallback& fn) {
        closed_fn_ = fn;
    }

    void setSocketReadTimeoutSeconds(const unsigned int seconds) {
        timeout_seconds_ = seconds;
    }

private:
    void startSocketReadTimedout();

    void stopSocketReadTimedout();

    void netSocketReadHandle(const asio::error_code& ec, std::size_t bytes_transferred);

    void netSocketWriteHandle(const std::string& message);

private:
    asio::ip::tcp::socket socket_;

    asio::streambuf streambuf_;

    asio::steady_timer steady_timer_;

    ConnectionManager& conn_manager_;

    unsigned int timeout_seconds_;

    int32_t data_length_;

    any context_;

    ConnectionCallback conn_fn_;

    ReadMessageCallback read_fn_;

    WriteCompleteCallback write_complete_fn_;

    TimedoutCallback timedout_fn_;

    ClosedCallback closed_fn_;
};

} /* end namespace jsonrpc */

#endif /* __JSONRPC_CONNECTION_HPP__ */

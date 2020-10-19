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

#ifndef __JSONRPC_CONNECTION_MANAGER_HPP__
#define __JSONRPC_CONNECTION_MANAGER_HPP__

#include <mutex>
#include <unordered_set>

namespace jsonrpc {

class ConnectionManager : private asio::noncopyable {
public:
    ConnectionManager(bool http_protocol);

    ~ConnectionManager();

public:
    void start(connection_ptr conn);

    void stop(connection_ptr conn);

    void stopAll();

    bool protocol() const;

private:
    bool use_http_protocol_;

    std::mutex mutex_;

    std::unordered_set<connection_ptr> connections_;
};

} /* end namespace jsonrpc */

#endif /* __JSONRPC_CONNECTION_MANAGER_HPP__ */

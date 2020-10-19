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

#include <algorithm>

#include "connection.hpp"
#include "connection_manager.hpp"

namespace jsonrpc {

ConnectionManager::ConnectionManager(bool http_protocol)
    : use_http_protocol_(http_protocol) {

}

ConnectionManager::~ConnectionManager() {

}

void ConnectionManager::start(connection_ptr conn) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        connections_.insert(conn);
    }

    conn->start();
}

void ConnectionManager::stop(connection_ptr conn) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        connections_.erase(conn);
    }

    conn->stop(true);
}

void ConnectionManager::stopAll() {
    std::for_each(connections_.begin(), connections_.end(),
        std::bind(&Connection::stop, std::placeholders::_1, true));

    connections_.clear();
}

bool ConnectionManager::protocol() const {
    return use_http_protocol_;
}

} /* end namespace jsonrpc */

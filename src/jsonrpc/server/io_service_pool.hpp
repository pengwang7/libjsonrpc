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

#ifndef __JSONRPC_IO_SERVICE_POOL_HPP__
#define __JSONRPC_IO_SERVICE_POOL_HPP__

#include <vector>
#include <thread>

#include <asio/io_service.hpp>

namespace jsonrpc {

class IOServicePool : private asio::noncopyable {
public:
    explicit IOServicePool(std::size_t pool_size);

    void run();

    void stop();

    asio::io_service& getIOService();

    std::size_t size() { return io_services_.size(); }

private:
    typedef std::shared_ptr<asio::io_service> io_service_ptr;
    typedef std::shared_ptr<asio::io_service::work> work_ptr;
    typedef std::shared_ptr<std::thread> thread_ptr;

    std::vector<io_service_ptr> io_services_;

    std::vector<work_ptr> works_;

    std::size_t next_io_service_;
};

} /* end namespace jsonrpc */

#endif /* __JSONRPC_IO_SERVICE_POOL_HPP__ */

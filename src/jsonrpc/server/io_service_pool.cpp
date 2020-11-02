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

#include <sys/prctl.h>

#include <thread>
#include <stdexcept>

#include "io_service_pool.hpp"

namespace jsonrpc {

IOServicePool::IOServicePool(std::size_t pool_size) {
    if (pool_size == 0) {
        std::runtime_error("The IO Service pool size is 0");
    }

    next_io_service_ = 0;

    for (std::size_t i = 0; i < pool_size; ++ i) {
        io_service_ptr io_service(new asio::io_service(ASIO_CONCURRENCY_HINT_1));
        work_ptr work(new asio::io_service::work(*io_service));
        io_services_.push_back(io_service);
        works_.push_back(work);
    }
}

void IOServicePool::run() {
    std::vector<thread_ptr> threads;
    for (std::size_t i = 0; i < io_services_.size(); ++ i) {
        thread_ptr thread(new std::thread(
            [i, this]() -> void {
                std::string thread_name = std::string("io-thread-") + std::to_string(i);
                ::prctl(PR_SET_NAME, thread_name.c_str(), 0, 0, 0);
                io_services_[i]->run();
            })
        );

        threads.push_back(thread);
    }

    for (std::size_t i = 0; i < io_services_.size(); ++ i) {
        threads[i]->join();
    }
}

void IOServicePool::stop() {
    for (std::size_t i = 0; i < io_services_.size(); ++ i) {
        io_services_[i]->stop();
    }
}

asio::io_service& IOServicePool::getIOService() {
    asio::io_service& io_service = *io_services_[next_io_service_];

    ++ next_io_service_;

    if (next_io_service_ == io_services_.size()) {
        next_io_service_ = 0;
    }

    return io_service;
}

} /* end namespace jsonrpc */

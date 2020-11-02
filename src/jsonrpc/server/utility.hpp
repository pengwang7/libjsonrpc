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

#ifndef __JSONRPC_UTILITY_HPP__
#define __JSONRPC_UTILITY_HPP__

#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>

#include "jsoncpp/json.h"

namespace jsonrpc {

#define RPC_ID          "id"
#define RPC_METHOD      "method"
#define RPC_PARAMS      "params"
#define RPC_VERSION     "jsonrpc"

#define STR_WHITESPACE  " \t\n\r\v\f"

class RequestMessage {
public:
    static bool parse(std::istream &stream, std::unordered_map<std::string, std::string>& headers, bool use_http_protocol) {
        if (!use_http_protocol) {
            return true;
        }

        bool request_line = false;
        std::string line;
        std::string::size_type begin = std::string::npos;
        std::string::size_type end = std::string::npos;

        while (std::getline(stream, line) && line != "\r") {
            std::string::size_type index = line.find(":", 1);
            if (index == std::string::npos) {
                if (request_line) {
                    return false;
                }

                // Request line or others error.
                headers["request_line"] = line;
                request_line = true;
                continue;
            }

            std::string field = line.substr(0, index);
            std::string value = line.substr(index + 1, -1);

            begin = field.find_first_not_of(STR_WHITESPACE);
            end = field.find_last_not_of(STR_WHITESPACE);
            field = field.substr(begin, end - begin + 1);

            begin = value.find_first_not_of(STR_WHITESPACE);
            end = value.find_last_not_of(STR_WHITESPACE);
            value = value.substr(begin, end - begin + 1);

            std::transform(field.begin(), field.end(), field.begin(), ::tolower);
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);

            headers[field] = value;
        }

        return true;
    }
};

class ResponseMessage {
public:
    static std::string message(std::string& content, bool use_http_protocol) {
        std::string message;

        if (use_http_protocol) {
            std::string status_code = "200";
            std::string status_description = "OK";
            std::string content_length = "Content-Length: ";
            std::string content_type = "Content-Type: ";
            std::string connection = "Connection: Keep-Alive";
            std::string application = "application/json";
            std::string crlf = "\r\n";

            message = "HTTP/1.1 " + status_code + status_description + crlf;
            message += content_length + std::to_string(content.size()) + crlf;
            message += content_type + application + crlf;
            message += crlf;

            message += content;
        } else {

        }

        return message;
    }
};

class Codec {
public:
    static std::string encode(const Json::Value& table) {
        std::string res("");
        if (table == Json::Value::null) {
            return res;
        }

        std::ostringstream out;
        Json::StreamWriterBuilder streamBuilder;
        std::unique_ptr<Json::StreamWriter> streamWriter(streamBuilder.newStreamWriter());
        streamWriter->write(table, &out);
        res = out.str();

        return res;
    }

    static int decode(const std::string str, Json::Value& table) {
        JSONCPP_STRING errs;
        Json::CharReaderBuilder charBuilder;
        std::unique_ptr<Json::CharReader> charReader(charBuilder.newCharReader());
        int res = charReader->parse(str.c_str(), str.c_str() + str.length(), &table, &errs);
        if (!res || !errs.empty()) {
            return -1;
        }

        return 0;
    }
};

} /* end namespace jsonrpc */

#endif /* __JSONRPC_UTILITY_HPP__ */
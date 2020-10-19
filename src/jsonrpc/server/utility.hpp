#ifndef __JSONRPC_UTILITY_HPP__
#define __JSONRPC_UTILITY_HPP__

#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>

namespace jsonrpc {

#define STR_WHITESPACE " \t\n\r\v\f"

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

} /* end namespace jsonrpc */

#endif /* __JSONRPC_UTILITY_HPP__ */
#include <sstream>

#include "./request.h"
#include "../teapot_server.h"
#include <cl9s/teapot.h>

namespace cl9s
{
    request::request()
    {
    }

    request::~request()
    {
    }

    const bool request::set(const std::string& buffer) {
        std::size_t body_start_idx = buffer.find_first_of("\r\n\r\n");
        if (body_start_idx == std::string::npos) {
            return false; // invalid header (bad reqeust)
        }

        if (!parse_header(buffer.substr(0, body_start_idx))) {
            return false; // invalid header (bad_request)
        }

        if (body_start_idx + 1 < buffer.length()) {
            if (!parse_body(buffer.substr(body_start_idx + (std::size_t)1))) {
                return false; // bad request
            }
        }

        return true;
    }

    const bool request::parse_header(const std::string& header) {
        std::istringstream header_stream{header};
        std::string line;

        if (!std::getline(header_stream, line)) {
            return false; // invalid header
        }

        std::istringstream head_stream{line};
        std::string method_str;
        if (!std::getline(head_stream, method_str, ' ')) {
            return false; // invalid header
        }
        m_method = teapot::str_to_request_method(method_str);

        std::string href;
        std::size_t query_begin_idx;
        if (!std::getline(head_stream, href, ' ')) {
            return false; // invalid header
        }

        query_begin_idx = href.find_first_of('?');
        if (query_begin_idx != std::string::npos) {
            m_location = href.substr(0, query_begin_idx);
            m_querystring = href.substr(query_begin_idx + (std::size_t)1);
        } else {
            m_location = href;
        }

        for (line; std::getline(header_stream, line);) {
            std::cout << line << std::endl;
        }

        return true;
    }

    const int request::parse_body(const std::string& body) {
        return 0;
    }

    const std::string& request::get_querystring() const {
        return m_querystring;
    }

    const std::string& request::get_location() const {
        return m_location;
    }

    const std::string& request::get_content() const {
        return m_content;
    }

    const request_method& request::get_method() const {
        return m_method;
    }
}

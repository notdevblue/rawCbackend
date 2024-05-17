#include <sstream>

#include "./request.h"
#include "../teapot_server.h"
#include <cl9s/teapot.h>

namespace cl9s
{
    request::request() {
        m_querystring = std::map<const std::string, std::string>();
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

        // request method
        std::istringstream head_stream{line};
        std::string method_str;
        if (!std::getline(head_stream, method_str, ' ')) {
            return false; // invalid header
        }
        m_method = teapot::str_to_request_method(method_str);

        // request href
        std::string href;
        if (!std::getline(head_stream, href, ' ')) {
            return false; // invalid header
        }

        // location
        const std::size_t query_begin_idx = href.find_first_of('?');
        m_location = href.substr(0, query_begin_idx); // default is npos

        // querystring
        if (query_begin_idx != std::string::npos) {
            parse_querystring(href.substr(query_begin_idx + (std::size_t)1));
        }

        // etc
        for (line; std::getline(header_stream, line);) {
            std::cout << line << std::endl; // TODO: map it
        }

        return true;
    }

    const int request::parse_body(const std::string& body) {
        return 0;
    }

    void request::parse_querystring(const std::string& querystring) {
        std::istringstream querystring_stream{querystring};
        std::string key_value_string;

        for (key_value_string; std::getline(querystring_stream, key_value_string, '&');) {
            std::size_t seperator_idx = key_value_string.find_first_of('=');
            if (seperator_idx == std::string::npos) {
                continue;
            }

            m_querystring[key_value_string.substr(0, seperator_idx)] = key_value_string.substr(seperator_idx + 1);
        }

        m_querystring["NULL"] = "";
    }

    const std::string& request::get_querystring(const std::string& key) const {
        auto iter = m_querystring.find(key);
        if (iter == m_querystring.end()) {
            return m_querystring.at("NULL");
        }

        return m_querystring.at(key);
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

#include <sstream>

#include "./request.h"
#include "../teapot_server.h"
#include <cl9s/teapot.h>

namespace cl9s
{
    request::request() : m_body_followed(false)
                       , m_querystring(std::map<const std::string, std::string>())
                       , m_header_contents(std::map<const std::string, std::string>())
    {
        m_querystring["NULL"] = "";
    }

    request::~request()
    {
    }

    const int request::set(const std::string& buffer) {
        std::size_t body_start_idx = buffer.find("\r\n\r\n");

        if (body_start_idx == std::string::npos) {
#ifdef CONSOLE_LOG
            puts("Body followed after header.");
#endif
            m_body_followed = true;
        }

        if (parse_header(buffer.substr(0, body_start_idx)) != EXIT_SUCCESS) {
#ifdef CONSOLE_LOG
            puts("Header parse failed.");
#endif
            return EXIT_FAILURE; // invalid header (bad_request)
        }

        if (body_start_idx + 1 < buffer.length()) {
            if (parse_body(buffer.substr(body_start_idx + (std::size_t)1)) != EXIT_SUCCESS) {
#ifdef CONSOLE_LOG
                puts("Body parse failed.");
#endif
                return EXIT_FAILURE; // bad request
            }
        }

        return EXIT_SUCCESS;
    }

    const int request::parse_header(const std::string& header) {
        std::istringstream header_stream{header};
        std::string line;

        if (!std::getline(header_stream, line)) {
            return EXIT_FAILURE; // invalid header
        }

        // request method
        std::istringstream head_stream{line};
        std::string method_str;
        if (!std::getline(head_stream, method_str, ' ')) {
            return EXIT_FAILURE; // invalid header
        }
        m_method = teapot::str_to_request_method(method_str);

        // request href
        std::string href;
        if (!std::getline(head_stream, href, ' ')) {
            return EXIT_FAILURE; // invalid header
        }

        // location
        const std::size_t query_begin_idx = href.find_first_of('?');
        m_location = href.substr(0, query_begin_idx); // default is npos

        // querystring
        if (query_begin_idx != std::string::npos) {
            parse_querystring(href.substr(query_begin_idx + (std::size_t)1));
        }

        // etc
        std::size_t kv_seperator_idx;
        for (; std::getline(header_stream, line);) {
            if ((kv_seperator_idx = line.find_first_of(':')) == std::string::npos) {
                continue;
            }

            m_header_contents[line.substr(0, kv_seperator_idx)] = line.substr(kv_seperator_idx + 1);
        }

        return EXIT_SUCCESS;
    }

    const int request::parse_body(const std::string& body) {
        std::string content_type = get_querystring("Content-type");
        if (content_type.compare("") != 0) {
#ifdef CONSOLE_LOG
            puts("Content-type not included in header.");
#endif
            return EXIT_FAILURE;
        }

        // TODO: parse

        return EXIT_SUCCESS;
    }

    void request::parse_querystring(const std::string& querystring) {
        std::istringstream querystring_stream{querystring};
        std::string key_value_string;

        for (; std::getline(querystring_stream, key_value_string, '&');) {
            std::size_t seperator_idx = key_value_string.find_first_of('=');
            if (seperator_idx == std::string::npos) {
                continue;
            }

            m_querystring[key_value_string.substr(0, seperator_idx)] = key_value_string.substr(seperator_idx + 1);
        }
    }

    const std::string& request::get_querystring(const char* key) const {
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

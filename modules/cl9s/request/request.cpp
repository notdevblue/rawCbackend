#include <sstream>

#include "./request.h"
#include "../teapot_server.h"
#include <cl9s/teapot.h>

namespace cl9s
{
    request::request() : m_body_followed(false)
                       , m_querystring(std::map<const std::string, std::string>())
                       , m_header_contents(std::map<const std::string, std::string>())
                       , m_body(std::map<const std::string, std::string>())
    {
    }

    request::~request()
    {
    }

    const int request::set(const std::string& buffer) {
        const std::size_t body_start_idx = buffer.find("\n\n");

        if (parse_header(buffer.substr(0, body_start_idx)) != EXIT_SUCCESS) {
#ifdef CONSOLE_LOG
            puts("Header parse failed.");
#endif
            return EXIT_FAILURE; // invalid header (bad_request)
        }

        if (get_header_content("Content-Type") == nullptr) {
            return EXIT_SUCCESS; // no body
        }

        if (buffer.length() <= body_start_idx + (std::size_t)2) {
            m_body_followed = true;
            return EXIT_SUCCESS;
        }

        return parse_body(buffer.substr(body_start_idx + (std::size_t)2));
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
            parse_querystring_like(href.substr(query_begin_idx + (std::size_t)1), m_querystring);
        }

        // etc
        std::size_t kv_seperator_idx;
        for (; std::getline(header_stream, line);) {
            if ((kv_seperator_idx = line.find_first_of(':')) == std::string::npos) {
                continue;
            }

            // Header: Contents
            m_header_contents[line.substr(0, kv_seperator_idx)] = line.substr(kv_seperator_idx + 2);
        }

        return EXIT_SUCCESS;
    }

    const int request::parse_body(const std::string& body) {
        const std::string* content_type = get_header_content("Content-Type");
        const std::size_t additional_attrib_idx = content_type->find_first_of(';');
        const std::string type = content_type->substr(0, additional_attrib_idx);

        if (type.compare("application/x-www-form-urlencoded") == 0) {
            parse_www_form_urlencoded(body);
            return EXIT_SUCCESS;
        } else if (type.compare("multipart/form-data") == 0) {
            return parse_form_data(body, "boundary");
        }

        // unprocessed
        m_content = body;

        return EXIT_SUCCESS;
    }

    void request::parse_querystring_like(const std::string& source, std::map<const std::string, std::string>& to) {
        std::istringstream querystring_stream{source};
        std::string key_value_string;

        // FIXME: handle when there is no &
        for (; std::getline(querystring_stream, key_value_string, '&');) {
            std::size_t seperator_idx = key_value_string.find_first_of('=');
            if (seperator_idx == std::string::npos) {
                continue;
            }
            
            // querystring=data
            to[key_value_string.substr(0, seperator_idx)] = key_value_string.substr(seperator_idx + 1);
        }
    }
    
    const std::string* request::get_querystring(const char* key) const {
        auto iter = m_querystring.find(key);
        if (iter == m_querystring.end()) {
            return nullptr;
        }

        return &m_querystring.at(key);
    }

    const std::string* request::get_header_content(const char* key) const {
        auto iter = m_header_contents.find(key);
        if (iter == m_header_contents.end()) {
            return nullptr;
        }

        return &m_header_contents.at(key);
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

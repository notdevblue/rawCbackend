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
        m_header_contents["NULL"] = "";
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
            parse_querystring_like(href.substr(query_begin_idx + (std::size_t)1), m_querystring);
        }

        // etc
        std::size_t kv_seperator_idx;
        for (; std::getline(header_stream, line);) {
            if ((kv_seperator_idx = line.find_first_of(':')) == std::string::npos) {
                continue;
            }

            m_header_contents[line.substr(0, kv_seperator_idx)] = line.substr(kv_seperator_idx + 2);
        }

        return EXIT_SUCCESS;
    }

    const int request::parse_body(const std::string& body) {
        std::string content_type = get_header_content("Content-Type");
        if (content_type.compare("") == 0) {
            return EXIT_SUCCESS; // no body
        }
        
        std::cout << content_type << "\nbody:\n" << body << std::endl;

        std::size_t additional_attrib_idx = content_type.find_first_of(';');
        const std::string type = content_type.substr(0, additional_attrib_idx);

        if (type.compare("multipart/x_www_form_urlencoded")) {
            return parse_www_form_urlencoded(body);
        } else if (type.compare("multipart/form-data")) {
            return parse_form_data(body, "boundary");
        }

        // unprocessed
        m_content = body;

        return EXIT_SUCCESS;
    }

    void request::parse_querystring_like(const std::string& source, std::map<const std::string, std::string>& to) {
        std::istringstream querystring_stream{source};
        std::string key_value_string;

        for (; std::getline(querystring_stream, key_value_string, '&');) {
            std::size_t seperator_idx = key_value_string.find_first_of('=');
            if (seperator_idx == std::string::npos) {
                continue;
            }

            to[key_value_string.substr(0, seperator_idx)] = key_value_string.substr(seperator_idx + 1);
        }
    }
    
    const std::string& request::get_querystring(const char* key) const {
        auto iter = m_querystring.find(key);
        if (iter == m_querystring.end()) {
            return m_querystring.at("NULL");
        }

        return m_querystring.at(key);
    }

    const std::string& request::get_header_content(const char* key) const {
        auto iter = m_header_contents.find(key);
        if (iter == m_header_contents.end()) {
            return m_header_contents.at("NULL");
        }

        return m_header_contents.at(key);
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

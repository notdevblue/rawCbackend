#include <cl9s/request/request.h>
#include <iostream>
#include <sstream>

namespace cl9s
{
    void request::parse_www_form_urlencoded(const std::string& body) {
        parse_querystring_like(body, m_body);
    }

    void parse(const std::string&, const std::string&);

    void request::parse_form_data(const std::string& body, const std::string& boundary) {
        const std::size_t current_boundary_index = body.find(boundary);
        if (current_boundary_index == std::string::npos) {
            return;
        }

        const std::string temp_content_str = body.substr(current_boundary_index + boundary.length() + 1UL);
        const std::size_t next_boundary_index = temp_content_str.find(boundary);
        if (next_boundary_index == std::string::npos || next_boundary_index < 3UL) {
            return;
        }

        const std::string content = temp_content_str.substr(0UL, next_boundary_index - 2UL);

        const std::size_t name_index = content.find("name=");
        if (name_index == std::string::npos) {
            parse_form_data(temp_content_str, boundary); // continue;
            return;
        }

        const std::size_t name_end_index = content.find('\n', name_index);
        if (name_end_index == std::string::npos) {
            parse_form_data(temp_content_str, boundary); // continue;
            return;
        }

        const std::size_t subcontent_begin_index = content.find("\n\n");
        if (subcontent_begin_index == std::string::npos) {
            parse_form_data(temp_content_str, boundary); // continue;
            return;
        }

        m_body[content.substr(name_index + 6UL, name_end_index - (name_index + 6UL) - 1UL)]
            = content.substr(subcontent_begin_index + 2UL);

        parse_form_data(temp_content_str, boundary);
        return;
    }
}

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

        const std::string temp_content_str = body.substr(current_boundary_index + boundary.length() + 1);
        const std::size_t next_boundary_index = temp_content_str.find(boundary);
        if (next_boundary_index == std::string::npos || next_boundary_index < 3L) {
            return;
        }

        const std::string content = temp_content_str.substr(0, next_boundary_index - 3);

        std::cout << content << std::endl;

        // content-disposition: form-data; name="asfdasdf"; ...
        // name="asdasdf" -> m_body[name] = data;

        const std::size_t name_index = content.find("name="); // get name -> name="<this>"\n
        const std::size_t subcontent_begin_index = content.find("\n\n"); // get content -> \n\n<content>

        parse_form_data(temp_content_str, boundary);
        return;
    }
}

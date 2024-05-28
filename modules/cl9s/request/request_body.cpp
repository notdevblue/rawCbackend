#include <cl9s/request/request.h>
#include <iostream>
#include <sstream>

namespace cl9s
{
    void request::parse_www_form_urlencoded(const std::string& body) {
        parse_querystring_like(body, m_body);
    }

    const int request::parse_form_data(const std::string& body, const std::string& boundary) {
        std::istringstream content_stream{body};
        std::string content;
        std::cout << boundary << std::endl;
        std::cout << body << std::endl;

        // split with boundary
        // content-disposition: form-data; name="asfdasdf"; ...
        // name="asdasdf" -> m_body[name] = data;

        return EXIT_SUCCESS;
    }
}

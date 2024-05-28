#include <cl9s/request/request.h>
#include <iostream>

namespace cl9s
{
    void request::parse_www_form_urlencoded(const std::string& body) {

        std::cout << "body:" << body << std::endl;

        parse_querystring_like(body, m_body);
    }

    const int request::parse_form_data(const std::string& body, const std::string& boundary) {

        // split with boundary
        // content-disposition: form-data; name="asfdasdf"; ...
        // name="asdasdf" -> m_body[name] = data;

        return EXIT_SUCCESS;
    }
}

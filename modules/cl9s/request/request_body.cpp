#include <cl9s/request/request.h>

namespace cl9s
{
    const int request::parse_www_form_urlencoded(const std::string& body) {
        // parse_querystring_like(body, );

        return EXIT_SUCCESS;
    }

    const int request::parse_form_data(const std::string& body, const std::string& boundary) {

        // split with boundary
        // content-disposition: asdf; ...
        // content-type?

        return EXIT_SUCCESS;
    }
}

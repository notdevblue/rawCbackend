#include "./request.h"
#include "../teapot_server.h"

namespace cl9s::request
{
    const std::string& req::get_querystring() const {
        return m_querystring;
    }

    const std::string& req::get_location() const {
        return m_location;
    }

    const std::string& req::get_content() const {
        return m_content;
    }
}

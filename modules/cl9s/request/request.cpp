#include "./request.h"
#include "../teapot_server.h"

namespace cl9s::request
{
    req::req(const std::string location, const std::string querystring, const std::string body)
        : m_location(location), m_querystring(querystring), m_content(body) {
    }

    req::~req()
    {
    }

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

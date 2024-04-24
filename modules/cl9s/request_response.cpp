#include "teapot_server.h"

namespace cl9s
{
    const std::string& request::get_querystring() const {
        return m_querystring;
    }

    const std::string& request::get_location() const {
        return m_location;
    }

    const std::string& request::get_content() const {
        return m_content;
    }

    const sock& response::get_client() const {
        return m_client_socket;
    }

    void response::send_response(const char* response, const size_t& size) const {
        teapot_server::send(m_client_socket, response, size);
    }
}

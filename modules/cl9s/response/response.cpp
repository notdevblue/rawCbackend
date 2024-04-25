#include "response.h"
#include "../teapot_server.h"

namespace cl9s::response
{
    const sock& res::get_client() const {
        return m_client_socket;
    }

    res::res(sock& client_socket) : m_client_socket(client_socket) {
    }

    res::~res() {
        cl9s::teapot_server::close_connection(m_client_socket);
    }

    void res::send(const std::string& data, const int& statuscode) const {
        
    }
}
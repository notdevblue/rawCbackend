#include "response.h"
#include "../teapot_server.h"

#ifdef CONSOLE_LOG
#include <iostream>
#endif

namespace cl9s::response
{

    res::res(sock& client_socket) : m_client_socket(client_socket) {
    }

    res::~res() {
        cl9s::teapot::close_socket(m_client_socket);
    }

    const sock& res::get_client() const {
        return m_client_socket;
    }

    void res::send(const std::string& data, const status& code) const {
        std::string response_text
            = std::string{create_status_header(code)}
                .append("\nContent-Length: ")
                .append(std::to_string(data.length()))
                .append("\nContent-type: text/plain; charset=utf-8\n\n") // TODO: Content-type should be subject to change
                .append(data);

        #ifdef CONSOLE_LOG
        std::cout << "Response: \n" << response_text << std::endl;
        #endif

        teapot_server::send(m_client_socket, response_text.c_str(), response_text.length());
    }
}

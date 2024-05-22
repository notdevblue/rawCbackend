#include <cl9s/response/response.h>
#include <cl9s/teapot_server.h>
#include <file/filereader.h>
#include <time.h>
#include <string.h>

#ifdef CONSOLE_LOG
#include <iostream>
#endif

namespace cl9s
{

    response::response(sock& client_socket) : m_client_socket(client_socket) {
    }

    response::~response() {
    }

    const sock& response::get_client() const {
        return m_client_socket;
    }

    const int response::send(const content::contents& data, const status& code) const {
        time_t t;
        char date[37];

        (void)time(&t);
        tm* tm = localtime(&t);

        (void)strftime(date, sizeof(date), "Date: %a, %d %b %Y %H:%M:%S KST\n", tm);

        std::string response_text = std::string{create_status_header(code)}
                                        .append("Connection: Keep-Alive\n")
                                        .append(data.get_header_str())
                                        .append("Keep-Alive: timeout=5, max=1000\nServer: raw cxx\n")
                                        .append(date)
                                        .append("\n")
                                        .append(data.get_data());

        return teapot_server::send(m_client_socket, response_text.c_str(), response_text.length());
    }
}

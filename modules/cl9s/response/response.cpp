#include "response.h"
#include "../teapot_server.h"
#include <time.h>
#include <string.h>

#include "../../file/filereader.h"

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

    const std::string response::get_date_string() const {
        time_t t;
        tm* tm;
        char date[37];

        time(&t);
        tm = localtime(&t);

        strftime(date, sizeof(date), "Date: %a, %d %b %Y %H:%M:%S KST\n", tm);

        return date;
    }

    const int response::send(const content::contents& data, const status& code) const {
        std::string response_text = std::string{create_status_header(code)}
                                        .append("Connection: Keep-Alive\n")
                                        .append(data.get_header_str())
                                        .append("Keep-Alive: timeout=5, max=1000\nServer: raw cxx\n")
                                        .append(get_date_string())
                                        .append("\n")
                                        .append(data.get_data());

        return teapot_server::send(m_client_socket, response_text.c_str(), response_text.length());
    }
}

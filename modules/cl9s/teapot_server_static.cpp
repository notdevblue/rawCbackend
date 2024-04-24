#include "teapot_server.h"

namespace cl9s
{
    const int teapot_server::close_connection(const sock& client_socket, const int& how) {
        if (shutdown(client_socket, how)) {
            perror("teapot_server::close_connection() > shutdown");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const int teapot_server::send(
        const sock& client_socket,
        const char* response,
        const size_t& response_size) 
    {
        if (write(client_socket, response, response_size) < 0) {
            perror("teapot_server::send() > write");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
}

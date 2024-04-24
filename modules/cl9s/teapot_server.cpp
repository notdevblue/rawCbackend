#include <stdio.h>
#include <unistd.h>
#include <cstdlib>

#include "teapot_server.h"

namespace cl9s
{
    const int teapot_server::listen_connection(
        const sock& listening_socket,
        const int& address_family) const
    {
        sockaddr_in serverAddr;
        {
            serverAddr.sin_family = address_family;
            serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            serverAddr.sin_port = htons(m_port);
        };

        if (bind(listening_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("teapot_server::listen_connection() > bind");
            return EXIT_FAILURE;
        }

        if (listen(listening_socket, SOMAXCONN) < 0) {
            perror("teapot_server::listen_connection() > listen");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const int teapot_server::accept_client(
        const sock& listening_socket,
        sock* client_socket OUT)
    {
        sockaddr_in clientAddr;
        socklen_t clientLength;

        clientLength = sizeof(clientAddr);
        *client_socket = accept(listening_socket, (sockaddr*)&clientAddr, &clientLength);

        if (*client_socket < 0) {
            perror("teapot_server::accept_client() > accept");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const int teapot_server::receive(
        const sock& client_socket,
        char* buffer,
        const size_t& buffer_size) const
    {
        ssize_t received;

        do {
            received = read(client_socket, buffer, buffer_size);
        } while(received > 0 && buffer[received] > 0);

        if (received < 0) {
            perror("teapot_server::receive() > read");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const bool teapot_server::is_client_alive(const sock& client_socket) const {
        return write(client_socket, NULL, 0) < 0;
    }

    std::unique_ptr<char[]> teapot_server::create_buffer(const int& size) const {
        return std::make_unique<char[]>(size);
    }

    void teapot_server::route(const request_method& method, const std::string& href, const t_route_lambda& callback) {
        if (m_route[method][href] != NULL) {
            std::cout << "Overwriting route: " << href << std::endl;
        }

        m_route[method][href] = callback;
    }

    void teapot_server::send_404_error(const sock& client_socket) const {
        char res[] =
            "HTTP/1.1 404 Not Found\n"
            "Content-Length: 10\n"
            "Content-Type: text/plain; charset=utf-8\n"
            "\n"
            "Not found.\n";

        send(client_socket, res, sizeof(res));
        teapot_server::close_connection(client_socket);
    }
}

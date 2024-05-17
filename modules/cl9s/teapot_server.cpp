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
        
        timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        if (setsockopt(
                *client_socket,
                SOL_SOCKET,
                SO_RCVTIMEO,
                (const char*)&tv,
                sizeof(tv)) < 0) {
            perror("teapot_server::accept_client() > setsockopt (rcvtimeo)");
            close(*client_socket);
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    std::unique_ptr<char[]> teapot_server::create_buffer(const int& size) const {
        return std::make_unique<char[]>(size);
    }

    void teapot_server::route(const request_method& method, const std::string& href, const route_lambda_t& callback) {
        if (m_route[method][href] != NULL) {
            std::cout << "Overwriting route: " << href << std::endl;
        }

        m_route[method][href] = callback;
    }
}

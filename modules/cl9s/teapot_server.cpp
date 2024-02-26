#include <stdio.h>
#include <unistd.h>
#include <cstdlib>

#include "teapot_server.h"

namespace cl9s
{
    const int teapot_server::listen_connection(const int& address_family) const {
        sockaddr_in serverAddr;
        {
            serverAddr.sin_family = address_family;
            serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            serverAddr.sin_port = htons(m_port);
        };

        if (bind(m_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("teapot_server::listen_connection() > bind");
            return EXIT_FAILURE;
        }

        if (listen(m_socket, SOMAXCONN) < 0) {
            perror("teapot_server::listen_connection() > listen");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const int teapot_server::accept_client() {
        sockaddr_in clientAddr;
        socklen_t clientLength;

        clientLength = sizeof(clientAddr);
        m_client_socket = accept(m_socket, (sockaddr*)&clientAddr, &clientLength);

        if (m_client_socket < 0) {
            perror("teapot_server::accept_client() > accept");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const int teapot_server::send(const char* response, const size_t& response_size) const {
        if (write(m_client_socket, response, response_size) < 0) {
            perror("teapot_server::send() > write");
            return EXIT_FAILURE;
        }
        
        return EXIT_SUCCESS;
    }

    const int teapot_server::receive(char* buffer, const size_t& buffer_size) const {
        ssize_t received;

        do {
            received = read(m_client_socket, buffer, buffer_size);

        } while(received > 0 && buffer[received] > 0);

        if (received < 0) {
            perror("teapot_server::receive() > read");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const int teapot_server::close_connection(const int& how) const {
        if (shutdown(m_client_socket, how)) {
            perror("teapot_server::close_connection() > shutdown");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const bool teapot_server::is_client_alive() const {
        return write(m_client_socket, NULL, 0) < 0;
    }
}

#pragma once

#include "teapot.h"

namespace cl9s
{
    class teapot_server : public teapot 
    {
    public:
        teapot_server(const teapot_server&) = delete;

        teapot_server(const uint16_t& port, const int& reuse_address = 1) : teapot(false, reuse_address) {
            m_port = port;
        }

        virtual ~teapot_server() {

        }

    public:
        // Summary:
        //  bind socket and listen for connection.
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        const int listen_connection(const int& address_family = AF_INET) const {
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

        // Summary:
        //  accepts client connection
        //
        // Returns:
        //  client socket on Success
        //  EXIT_FAILURE on Fail
        const sock accept_client() const {
            int clientSocket;
            sockaddr_in clientAddr;
            socklen_t clientLength;

            clientLength = sizeof(clientAddr);
            clientSocket = accept(m_socket, (sockaddr*)&clientAddr, &clientLength);

            if (clientSocket < 0) {
                perror("teapot_server::accept_client() > accept");
                return EXIT_FAILURE;
            }

            return clientSocket;
        }

    private:
        uint16_t m_port;
    };
};

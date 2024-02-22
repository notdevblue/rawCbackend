#pragma once

#include "teapot.h"
#include <thread>

namespace cl9s
{
    #pragma SERVER_BUFFER_SIZE 4096

    class teapot_server : public teapot 
    {
    public:
        teapot_server(const teapot_server&) = delete;

        teapot_server(const uint16_t& port, const int& reuse_address = 1) : teapot(false, reuse_address) {
            m_port = port;
        }

        virtual ~teapot_server() {
            close_connection();

            // m_connection_thread.join();
        }

    public:
        void handle_client_connection() {
            create_socket();
        }

        // Summary:
        //  bind socket and listen for connection.
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        const int listen_connection(const int& = AF_INET) const;

        // Summary:
        //  accepts client connection
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        const sock accept_client() {
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

        // Summary:
        //  send data to client
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        const int send(const char* response, const size_t& response_size) const {
            if (write(m_client_socket, response, response_size) < 0) {
                perror("teapot_server::send() > write");
                return EXIT_FAILURE;
            }
            
            return EXIT_SUCCESS;
        }

        // Summary:
        //  receives data from client
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILIRE on Fail
        const int receive(char* buffer, const size_t& buffer_size) const {
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

        // Summary:
        //  closes connection with client
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        const int close_connection(const int& how = SHUT_RDWR) {
            if (shutdown(m_client_socket, how)) {
                perror("teapot_server::close_connection() > shutdown");
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }

        // Summary:
        //  check connection whether it's alive or not
        // 
        // Returns:
        //  1 when connection is alive
        //  0 when connection is closed
        const bool is_client_alive() {
            return write(m_client_socket, NULL, 0) < 0;
        }

    private:
        uint16_t m_port;
        sock m_client_socket;
        std::thread m_connection_thread;
    };
};

#pragma once

#include "teapot.h"
#include <memory>
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
            m_bKeepAcceptConnection = true;
        }

        virtual ~teapot_server() {
            close_connection();

            if (m_connection_thread != nullptr)
                m_connection_thread.get()->join();
        }

    public:
        // Summary:
        //  handles client connection
        //  and invoke route funciton
        const std::shared_ptr<std::thread>& handle_client_connection();

        // Summary:
        //  bind socket and listen for connection.
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        const int listen_connection(
            const sock& listening_socket,
            const int& address_family = AF_INET) const;

        // Summary:
        //  accepts client connection
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        const int accept_client(const sock& listening_socket, sock* client_socket OUT);

        // Summary:
        //  send data to client
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        const int send(const char* response, const size_t& response_size) const;

        // Summary:
        //  receives data from client
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILIRE on Fail
        const int receive(char* buffer, const size_t& buffer_size) const;

        // Summary:
        //  closes connection with client
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        const int close_connection(const int& how = SHUT_RDWR) const;

        // Summary:
        //  check connection whether it's alive or not
        // 
        // Returns:
        //  1 when connection is alive
        //  0 when connection is closed
        const bool is_client_alive() const;

        virtual void stop(const int& how, const char* errmsg = "") override;

    protected:
        void handle_client_thread() ;

    private:
        uint16_t m_port;
        sock m_client_socket;

        bool m_bKeepAcceptConnection;

        std::shared_ptr<std::thread> m_connection_thread;
    };
};

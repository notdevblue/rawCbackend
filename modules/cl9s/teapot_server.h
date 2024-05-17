#pragma once

#include "teapot.h"
#include "./response/response.h"
#include "./request/request.h"

#include <iostream>
#include <memory>
#include <thread>
#include <string>
#include <functional>
#include <map>

namespace cl9s
{
#define SERVER_BUFFER_SIZE 4096

    typedef std::function<void(const request&&, const response&&)> route_lambda_t;

    class teapot_server : public teapot {
    public:
        teapot_server(const teapot_server&) = delete;

        teapot_server(const uint16_t& port, const int& reuse_address = 1) : teapot(false, reuse_address) {
            m_port = port;
            m_bKeepAcceptConnection = true;
            m_route = std::map<request_method, std::map<std::string, route_lambda_t>>();

            for (request_method req_method = (request_method)0;
                req_method < request_method::END_OF_ENUM;
                req_method = (request_method)((int)req_method + 1)) {
                m_route[req_method] = std::map<std::string, route_lambda_t>();
            }
        }

        virtual ~teapot_server() {
            if (m_connection_thread.get() != nullptr && m_connection_thread.get()->joinable()) {
                m_connection_thread.get()->join();
            }
        }

    public:

        void route(const request_method& method, const std::string& href, const route_lambda_t& callback);

        // Summary:
        //  handles client connection
        //  and invoke route funciton
        const std::shared_ptr<std::thread>& handle_client_connection(const bool& silent_startup = false);

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
        //  check connection whether it's alive or not
        // 
        // Returns:
        //  1 when connection is alive
        //  0 when connection is closed
        const bool is_client_alive(const sock& client_socket) const = delete;

        std::unique_ptr<char[]> create_buffer(const int& size = SERVER_BUFFER_SIZE) const;

        virtual void stop(const int& how, const char* errmsg = "") override;

    protected:
        void accept_client_thread();
        void handle_client_thread(sock client_socket);

        sock handle_create();
        const bool handle_listen(sock& socket IN);
        const bool handle_accept(const sock& listening_socket, sock& client_socket OUT);
        const bool handle_receive_header(sock& client_socket IN OUT, std::function<const bool(const std::string& buffer)> callback);

    private:
        uint16_t m_port;

        bool m_bKeepAcceptConnection;

        std::shared_ptr<std::thread> m_connection_thread;
        std::map<request_method, std::map<std::string, route_lambda_t>> m_route;
        std::map<request_method, std::map<std::string, route_lambda_t>>::iterator m_route_it;
        std::map<std::string, route_lambda_t>::iterator m_route_path_it;
    };
};

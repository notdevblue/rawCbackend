#include <cstdlib>
#include <thread>
#include <unistd.h>
#include <stdio.h>
#include <memory>

#include "teapot_server.h"

#define DEAFULT_FAILSAFE_COUNT 50

namespace cl9s
{
    const std::shared_ptr<std::thread>& teapot_server::handle_client_connection() {
        m_connection_thread = std::make_shared<std::thread>([this] { this->handle_client_thread(); });

        return m_connection_thread;
    }

    void teapot_server::stop(const int& how, const char* errmsg) {
        m_bKeepAcceptConnection = false;

        if (how == STOP_EXCEPTION) {
            printf("Called stop with exception\n");
            perror(errmsg);
            exit(EXIT_FAILURE);
            return;
        }
    }

    void teapot_server::handle_client_thread() {
        unsigned char creating_failsafe = 0;
        unsigned char listening_failsafe = 0;
        unsigned char accept_failsafe = 0;
        unsigned char receive_header_failsafe = 0;
        sock listening_socket;

        while (m_bKeepAcceptConnection) {
            if (create_socket(&listening_socket) < 0) {
                close(listening_socket);
                ++creating_failsafe;

                if (creating_failsafe >= DEAFULT_FAILSAFE_COUNT) {
                    stop(STOP_EXCEPTION, "Creating failsafe");
                    break;
                }
                continue;
            }
            creating_failsafe = 0;

            if (listen_connection(listening_socket) < 0) {
                close(listening_socket);
                ++listening_failsafe;

                if (listening_failsafe >= DEAFULT_FAILSAFE_COUNT) {
                    stop(STOP_EXCEPTION, "Listening failsafe");
                    break;
                }
                continue;
            }
            listening_failsafe = 0;

            // accept client
            sock client_socket;
            if (accept_client(listening_socket, &client_socket) < 0) {
                close(listening_socket);
                ++accept_failsafe;

                if (accept_failsafe >= DEAFULT_FAILSAFE_COUNT) {
                    stop(STOP_EXCEPTION, "Accept failsafe");
                    break;
                }
            }
            accept_failsafe = 0;

            close(listening_socket);
            std::unique_ptr<char[]> unique_buffer = create_buffer();
            char* buffer = unique_buffer.get();

            if (receive(client_socket, buffer, sizeof(buffer)) < 0) {
                close_connection(client_socket);
                ++receive_header_failsafe;

                if (receive_header_failsafe >= DEAFULT_FAILSAFE_COUNT) {
                    stop(STOP_EXCEPTION, "Receive header failsafe");
                    break;
                }
            }
            receive_header_failsafe = 0;

            char response[] =
            "HTTP/1.1 200 OK\n"
            "Content-Length: 22\n"
            "Content-Type: text/plain; charset=utf-8\n"
            "\n"
            "Install Gentoo Always!\n";

            send(client_socket, response, sizeof(response));
            close_connection(client_socket);

            // TODO: route

        } // while (m_bKeepAcceptConnection)

        printf("\n### handle client thread shutdown... ###\n\n");
    }
}

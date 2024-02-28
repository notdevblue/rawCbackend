#include <cstdlib>
#include <thread>
#include <unistd.h>
#include <stdio.h>

#include "teapot_server.h"

#define DEAFULT_FAILSAFE_COUNT 25

namespace cl9s
{
    const std::shared_ptr<std::thread>& teapot_server::handle_client_connection() {
        m_connection_thread = std::make_shared<std::thread>([this] { this->handle_client_connection(); });

        return m_connection_thread;
    }

    void teapot_server::stop(const int& how, const char* errmsg) {
        this->m_bKeepAcceptConnection = false;

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
        sock listening_socket;

        while (this->m_bKeepAcceptConnection) {
            if (this->create_socket(&listening_socket) < 0) {
                close(listening_socket);
                ++creating_failsafe;

                if (creating_failsafe >= DEAFULT_FAILSAFE_COUNT) {
                    this->stop(STOP_EXCEPTION, "Creating failsafe");
                    break;
                }
                continue;
            }
            creating_failsafe = 0;

            if (this->listen_connection(listening_socket) < 0) {
                close(listening_socket);
                ++listening_failsafe;

                if (listening_failsafe >= DEAFULT_FAILSAFE_COUNT) {
                    this->stop(STOP_EXCEPTION, "Listening failsafe");
                    break;
                }
                continue;
            }
            listening_failsafe = 0;

            // accept client
            sock client_socket;
            if (this->accept_client(listening_socket, &client_socket) < 0) {
                close(listening_socket);
                ++accept_failsafe;

                if (accept_failsafe >= DEAFULT_FAILSAFE_COUNT) {
                    this->stop(STOP_EXCEPTION, "Accept failsafe");
                    break;
                }
            }
            accept_failsafe = 0;
        }

        close(listening_socket);
    }
}

#include <cstdlib>
#include <string.h>
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

    sock teapot_server::handle_create() {
        static unsigned char creating_failsafe = 0;
        sock socket;

        if (create_socket(&socket) < 0) {
            close(socket);
            ++creating_failsafe;

            if (creating_failsafe >= DEAFULT_FAILSAFE_COUNT) {
                stop(STOP_EXCEPTION, "Creating failsafe");
            }

            return 0;
        }

        creating_failsafe = 0;
        return socket;
    }

    const bool teapot_server::handle_listen(const sock& socket) {
        static unsigned char listening_failsafe = 0;

        if (listen_connection(socket) < 0) {
            close(socket);
            ++listening_failsafe;

            if (listening_failsafe >= DEAFULT_FAILSAFE_COUNT) {
                stop(STOP_EXCEPTION, "Listening failsafe");
            }

            return false;
        }

        listening_failsafe = 0;

        return true;
    }

    const bool teapot_server::handle_accept(const sock& listening_socket, sock& client_socket OUT) {
        static unsigned char accept_failsafe = 0;

        if (accept_client(listening_socket, &client_socket) < 0) {
            close(listening_socket);
            ++accept_failsafe;

            if (accept_failsafe >= DEAFULT_FAILSAFE_COUNT) {
                stop(STOP_EXCEPTION, "Accept failsafe");
            }

            return false;
        }

        accept_failsafe = 0;
        close(listening_socket);

        return true;
    }

    const bool teapot_server::handle_receive_header(
        const sock& client_socket,
        std::function<void(const char* buffer, const int& len)> callback)
    {
        static unsigned char receive_header_failsafe = 0;

        std::unique_ptr<char[]> unique_buffer = create_buffer(); // FIXME: use shared buffer instead
        char* buffer = unique_buffer.get();

        if (receive(client_socket, buffer, SERVER_BUFFER_SIZE) < 0) {
            close_connection(client_socket);
            ++receive_header_failsafe;

            if (receive_header_failsafe >= DEAFULT_FAILSAFE_COUNT) {
                stop(STOP_EXCEPTION, "Receive header failsafe");
            }

            return false;
        }

        receive_header_failsafe = 0;

        callback(buffer, strlen(buffer));

        return true;
    }

    void teapot_server::handle_client_thread() {
        sock listening_socket;

        while (m_bKeepAcceptConnection) {

            // create listening socket
            if (!(listening_socket = handle_create())) {
                continue;
            }

            // listen for connections
            if (!handle_listen(listening_socket)) {
                continue;
            }

            // accept client
            sock client_socket;
            if (!handle_accept(listening_socket, client_socket)) {
                continue;
            }

            char* method;
            char* path;
            // receive header
            if (
                !handle_receive_header(
                    client_socket,
                    [&method, &path](auto buffer, auto len) {
                        char* saveptr1;
                        char* token;
                        char* copiedstr = strdup(buffer);

                        token = strtok_r(copiedstr, "\n", &saveptr1);
                        if (token == NULL) {
                            free(copiedstr);
                            return;
                        }

                        saveptr1 = nullptr;

                        method = strtok_r(token, " ", &saveptr1);
                        path = strtok_r(NULL, " ", &saveptr1);

                        free(copiedstr);
                    }
                )
            )
            {
                continue;
            }

            std::cout << "method: " << method << " path: " << path << std::endl;
    
            request_method req_method = str_to_request_method(method);

            m_route_it = m_route.find(req_method);
            if (m_route_it == m_route.end()) {
                send_404_error(client_socket);
                continue;
            }

            const auto inner_map = &m_route[req_method];

            m_route_path_it = inner_map->find(path);
            if (m_route_path_it == inner_map->end()) {
                send_404_error(client_socket);
                continue;
            }

            request a = request("hello");
            response b = response(client_socket);
            inner_map->at(path)(a, b);

            close_connection(client_socket);
        } // while (m_bKeepAcceptConnection)

        printf("\n### handle client thread shutdown... ###\n\n");
    }
}

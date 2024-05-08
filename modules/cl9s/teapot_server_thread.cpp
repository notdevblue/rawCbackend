#include <cstdlib>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <stdio.h>
#include <memory>

#include "teapot_server.h"
#include "../strdup_raii/strdup_raii.h"

#define DEAFULT_FAILSAFE_COUNT 50

namespace cl9s
{
    const std::shared_ptr<std::thread>& teapot_server::handle_client_connection(const bool& silent_startup) {
        m_connection_thread = std::make_shared<std::thread>([this] { this->accept_client_thread(); });

        if (!silent_startup) {
            std::cout << "-----------------------------\n";
            std::cout << "server running on port: " << m_port << '\n';
            std::cout << "-----------------------------\n\n";
        }

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
        sock socket;
        if (create_socket(&socket) < 0) {
            close(socket);

            return 0;
        }

        return socket;
    }

    const bool teapot_server::handle_listen(sock& socket IN) {
        if (listen_connection(socket) != 0) {
            close(socket);
            return false;
        }

        return true;
    }

    const bool teapot_server::handle_accept(const sock& listening_socket, sock& client_socket OUT) {
        if (accept_client(listening_socket, &client_socket) != 0) {
            close(listening_socket);
            return false;
        }

        close(listening_socket);
        return true;
    }

    const bool teapot_server::handle_receive_header(
        sock& client_socket IN OUT,
        std::function<const bool(const char* buffer, const int& len)> callback)
    {
        std::unique_ptr<char[]> unique_buffer = create_buffer(); // FIXME: use shared buffer instead
        char* buffer = unique_buffer.get();

        if (receive(client_socket, buffer, SERVER_BUFFER_SIZE) != 0) {
            // remote closed connection
            return false;
        }

        return callback(buffer, strlen(buffer));
    }

    void teapot_server::accept_client_thread() {
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

            std::thread([this, &client_socket] {
                this->handle_client_thread(client_socket);
            }).detach();
        } // while (m_bKeepAcceptConnection)

        printf("\n### accept client thread shutdown... ###\n\n");
    }

    void teapot_server::handle_client_thread(sock client_socket) {
        while (true) {
            char method[7];
            strdup_raii path;

            if (
                !handle_receive_header(
                    client_socket,
                    [&method, &path](const char* buffer, const int& len) -> const bool {
                        char* saveptr1;
                        char* token;
                        strdup_raii copiedstr{buffer};

                        token = strtok_r(copiedstr.get(), "\n", &saveptr1);
                        if (token == NULL) {
                            return false;
                        }

                        saveptr1 = nullptr;

                        try {
                            strcpy(method, strtok_r(token, " ", &saveptr1));
                            path.assign(strtok_r(NULL, " ", &saveptr1));
                        } catch (...)  {
                            return false;
                        }

                        if (method == NULL || path.get() == NULL) {
                            return false;
                        }

                        return true;
                    })) {
                break; // close
            }

            request_method req_method = str_to_request_method(method);
            response::res res = response::res{client_socket};

            m_route_it = m_route.find(req_method);
            if (m_route_it == m_route.end()) {
                res.send("Not Found.", status::NOT_FOUND);
                continue;
            }

            const auto inner_map = &m_route[req_method];

            m_route_path_it = inner_map->find(path.get());
            if (m_route_path_it == inner_map->end()) {
                res.send("Not Found.", status::NOT_FOUND);
                continue;
            }

            inner_map->at(path.get())(request::req("hello"), std::move(res));
        }

        close_socket(client_socket);
    }
}

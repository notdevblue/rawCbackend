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
            int header_ok = 0;
            std::string path;
            std::shared_ptr<request::req> req;
            std::shared_ptr<response::res> res = std::make_shared<response::res>(client_socket);

            if (
                header_ok = handle_receive_header(
                    client_socket,
                    [&method, &path, &req](const char* buffer, const int& len) -> const bool {
                        char* saveptr1;
                        char* token;
                        std::string querystr;
                        strdup_raii copiedstr{buffer};

                        token = strtok_r(copiedstr.get(), "\n", &saveptr1);
                        if (token == NULL) {
#ifdef CONSOLE_LOG
                            puts("Invalid header.\n");
#endif
                            return 1;
                        }

                        saveptr1 = nullptr;

                        try {
                            char* tmp_method = strtok_r(token, " ", &saveptr1);
                            if (strlen(tmp_method) > 7 || tmp_method == nullptr) {
#ifdef CONSOLE_LOG
                                puts("Invalid request method.\n");
#endif
                                return 1;
                            }

                            strcpy(method, tmp_method);                                 // req method
                            std::string tmp_full_path = strtok_r(NULL, " ", &saveptr1); // path (including querystring)

                            int querystr_begin_idx = tmp_full_path.find_first_of('?');

                            path = tmp_full_path.substr(0, querystr_begin_idx);
                            querystr = tmp_full_path.substr(querystr_begin_idx + 1);
                        } catch (const std::exception& e) {
#ifdef CONSOLE_LOG
                            std::cerr << e.what() << std::endl;
                            puts("Header parse failed.\n");
#endif
                            return 2;
                        }

                        if (method == NULL || path.length() == 0) {
#ifdef CONSOLE_LOG
                            puts("Invalid request head.\n");
#endif
                            return 2;
                        }

                        req = std::make_shared<request::req>(path, querystr, buffer);
                        return 0;
                    }) != 0) {
                switch (header_ok) {
                    case 1:
                        res->send(content::text("Bad Request"), status::BAD_REQUEST);
                        break;
                    case 2:
                        res->send(content::text("Internal Server Error"), status::INTERNAL_SERVER_ERROR);
                        break;
                };
                break; // close
            }

            request_method req_method = str_to_request_method(method);

            m_route_it = m_route.find(req_method);
            if (m_route_it == m_route.end()) {
#ifdef CONSOLE_LOG
                printf("Request method <%s> not found.\n", method);
#endif
                res->send(content::text("Not Found."), status::NOT_FOUND);
                continue;
            }

            const auto inner_map = &m_route[req_method];

            m_route_path_it = inner_map->find(path);
            if (m_route_path_it == inner_map->end()) {
#ifdef CONSOLE_LOG
                printf("Request path <%s> not found.\n", path.c_str());
#endif
                res->send(content::text("Not Found."), status::NOT_FOUND);
                continue;
            }

            inner_map->at(path)(req.get(), res.get());
        }

        close_socket(client_socket);
    }
}

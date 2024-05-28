#include <cl9s/teapot_server.h>
#include <strdup_raii/strdup_raii.h>

namespace cl9s
{
    void teapot_server::handle_client_connection(const bool& silent_startup) {
        m_connection_thread = std::make_shared<std::thread>([this] { this->accept_client_thread(); });

        if (!silent_startup) {
            std::cout << "-----------------------------\n";
            std::cout << "server running on port: " << m_port << '\n';
            std::cout << "-----------------------------\n\n";
        }

        pause();
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

        puts("\n### accept client thread shutdown... ###\n");
    }

    void teapot_server::handle_client_thread(sock client_socket) {
#ifdef CONSOLE_LOG
        printf("socket %d connected.\n", client_socket);
#endif
        while (true) {
            request req = request();
            response res{client_socket};
            char buffer[SERVER_BUFFER_SIZE] = {0};

            if (receive(client_socket, buffer, SERVER_BUFFER_SIZE) != EXIT_SUCCESS) {
                break;
            }

            std::remove(buffer, buffer + (SERVER_BUFFER_SIZE - 1), '\r');

            if (req.set(buffer) != EXIT_SUCCESS) {
                (void)res.send_400();
                break;
            }

            // receive body if needed
            if (req.is_body_needs_parsing()) {
                const std::string* content_length_str = req.get_header_content("Content-Length");
                if (content_length_str == nullptr) {
#ifdef CONSOLE_LOG
                    puts("Content-Length not defined.");
#endif
                    (void)res.send_400();
                    break;
                }
                const int content_length = std::stoi(*content_length_str) + 1;
                char* body_buffer = (char*)malloc(content_length * sizeof(char));
                memset(body_buffer, 0, content_length);

                if (receive(client_socket, body_buffer, content_length) != EXIT_SUCCESS) {
                    (void)res.send_400();
                    free(body_buffer);
                    break;
                }

                if (req.parse_body(body_buffer) != EXIT_SUCCESS) {
                    (void)res.send_400();
                    free(body_buffer);
                    break;
                }

                free(body_buffer);
            }

            const request_method& method = req.get_method();
            m_route_it = m_route.find(method);
            if (m_route_it == m_route.end()) {
#ifdef CONSOLE_LOG
                puts("Request method not found.");
#endif
                if (res.send_404() == EXIT_FAILURE) {
                    break;
                }
                continue;
            }

            auto inner_map = m_route[method];
            const std::string& path = req.get_location();

            m_route_path_it = inner_map.find(path);
            if (m_route_path_it == inner_map.end()) {
#ifdef CONSOLE_LOG
                puts("Request path not found.");
#endif
                if (res.send_404() == EXIT_FAILURE) {
                    break;
                }
                continue;
            }

            inner_map.at(path)(std::move(req), std::move(res));
        }

        (void)close_socket(client_socket);
#ifdef CONSOLE_LOG
        printf("socket %d closed.\n", client_socket);
#endif
        return;
    }
}

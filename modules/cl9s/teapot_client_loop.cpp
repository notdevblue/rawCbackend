#include <cl9s/teapot_server.h>
#include "../strdup_raii/strdup_raii.h"

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

        (void)printf("\n### accept client thread shutdown... ###\n\n");
    }

    void teapot_server::handle_client_thread(sock client_socket) {
#ifdef CONSOLE_LOG
        printf("socket %d connected.\n", client_socket);
#endif
        while (true) {
            request req = request();
            response res{client_socket};

            if (handle_receive_header(client_socket, [&req](auto buf) { return req.set(buf); }) != EXIT_SUCCESS) {
                break; // close
            }

            const request_method& method = req.get_method();
            m_route_it = m_route.find(method);
            if (m_route_it == m_route.end()) {
#ifdef CONSOLE_LOG
                perror("Request method not found.\n");
#endif
                if (res.send(content::text("Not Found."), status::NOT_FOUND) == EXIT_FAILURE) {
                    break;
                }
                continue;
            }

            auto inner_map = m_route[method];
            const std::string path = req.get_location();

            m_route_path_it = inner_map.find(path);
            if (m_route_path_it == inner_map.end()) {
#ifdef CONSOLE_LOG
                perror("Request path not found.\n");
#endif
                if (res.send(content::text("Not Found."), status::NOT_FOUND) == EXIT_FAILURE) {
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

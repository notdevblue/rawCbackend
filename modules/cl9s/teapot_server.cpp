#include <cl9s/teapot_server.h>
#include "../strdup_raii/strdup_raii.h"

namespace cl9s
{
    const int teapot_server::listen_connection(
        const sock& listening_socket,
        const int& address_family) const
    {
        sockaddr_in serverAddr;
        {
            serverAddr.sin_family = address_family;
            serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            serverAddr.sin_port = htons(m_port);
        };

        if (bind(listening_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("teapot_server::listen_connection() > bind");
            return EXIT_FAILURE;
        }

        if (listen(listening_socket, SOMAXCONN) < 0) {
            perror("teapot_server::listen_connection() > listen");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const int teapot_server::accept_client(
        const sock& listening_socket,
        sock* client_socket OUT)
    {
        sockaddr_in clientAddr;
        socklen_t clientLength;

        clientLength = sizeof(clientAddr);
        *client_socket = accept(listening_socket, (sockaddr*)&clientAddr, &clientLength);

        if (*client_socket < 0) {
            perror("teapot_server::accept_client() > accept");
            return EXIT_FAILURE;
        }
        
        timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        if (setsockopt(
                *client_socket,
                SOL_SOCKET,
                SO_RCVTIMEO,
                (const char*)&tv,
                sizeof(tv)) < 0) {
            perror("teapot_server::accept_client() > setsockopt (rcvtimeo)");
            close(*client_socket);
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    void teapot_server::route(const request_method& method, const std::string& href, const route_lambda_t& callback) {
        if (m_route[method][href] != NULL) {
            std::cout << "Overwriting route: " << href << std::endl;
        }

        m_route[method][href] = callback;
    }

    void teapot_server::stop(const int& how, const char* errmsg) {
        m_bKeepAcceptConnection = false;

        m_connection_thread->detach();

        if (how == STOP_EXCEPTION) {
            (void)printf("Called stop with exception\n");
            perror(errmsg);
            exit(EXIT_FAILURE);
            return;
        }
    }

    sock teapot_server::handle_create() {
        sock socket;
        if (create_socket(&socket) < 0) {
            (void)close(socket);

            return 0;
        }

        return socket;
    }

    const bool teapot_server::handle_listen(sock& socket IN) {
        if (listen_connection(socket) != 0) {
            (void)close(socket);
            return false;
        }

        return true;
    }

    const bool teapot_server::handle_accept(const sock& listening_socket, sock& client_socket OUT) {
        if (accept_client(listening_socket, &client_socket) != 0) {
            (void)close(listening_socket);
            return false;
        }

        (void)close(listening_socket);
        return true;
    }

    const int teapot_server::handle_receive_header(
        sock& client_socket IN OUT,
        std::function<const int(const char* buffer)> callback) {
        char buffer[SERVER_BUFFER_SIZE] = {0};

        if (receive(client_socket, buffer, SERVER_BUFFER_SIZE) != EXIT_SUCCESS) {
            // remote closed connection
#ifdef CONSOLE_LOG
            puts("Non zero result");
#endif
            return EXIT_FAILURE;
        }

        return callback(buffer);
    }
}

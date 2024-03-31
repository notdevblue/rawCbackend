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

            // receive header
            if (
                !handle_receive_header(
                    client_socket,
                    [this](auto buffer, auto len) {
                        std::cout << "buffer:\n" << buffer << "\nlength: " << len << std::endl;
                    }
                )
            )
            {
                continue;
            }

            char res[] =
            "HTTP/1.1 200 OK\n"
            "Content-Length: 22\n"
            "Content-Type: text/plain; charset=utf-8\n"
            "\n"
            "Install Gentoo Always!\n";

            // TODO:
            /*
            요청 파싱해서 request method 랑 경로 아레에 넣고,
            request, response 객채 생성해서 등록된 그거 실행.

            없으면 404
            */

            request a = request("hello");
            response b;
            m_route[request_method::GET]["/"](a, b);

            send(client_socket, res, sizeof(res));
            close_connection(client_socket);

            // TODO: route

        } // while (m_bKeepAcceptConnection)

        printf("\n### handle client thread shutdown... ###\n\n");
    }
}

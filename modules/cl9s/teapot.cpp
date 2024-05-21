#include "teapot.h"
#include <signal.h>

#if CONSOLE_LOG
#include <iostream>
#include <string.h>
#endif

namespace cl9s
{
    teapot::teapot(
        const bool& prevent_socket_creation,
        const int& reuse_address) : reuse_address(reuse_address)
    {

        // request_method_from_string = {


        signal(SIGPIPE, SIG_IGN);
    }

    teapot::~teapot() 
    {
    }

// static public member functions
    const int teapot::send(
        const sock& client_socket,
        const char* response,
        const size_t& response_size)
    {
#if CONSOLE_LOG
        printf("sock: %d\n", client_socket);
#endif
        ssize_t res = ::send(client_socket, response, response_size, MSG_NOSIGNAL);

        if (res < 0) {
            perror("teapot_server::send()");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const int teapot::receive(
        const sock& client_socket,
        char* buffer,
        const size_t& buffer_size)
    {
        ssize_t received;

        do {
            received = read(client_socket, buffer, buffer_size);
            
#if CONSOLE_LOG
            printf("sock: %d, len: %ld, Request:\n%s\n", client_socket, received, buffer);
#endif
        } while (received > 0 && buffer[received] > 0);

        if (received < 0) {
            perror("teapot_server::receive()");
            
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const int teapot::close_socket(sock& socket IN, const int& how) {
        if (socket == 0) {
            return EXIT_SUCCESS;
        }

        if (shutdown(socket, how) < 0) {
            perror("teapot::close_socket()");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const request_method& teapot::str_to_request_method(const std::string& method) {
        static std::map<const std::string, request_method> local_map {
            {"GET", request_method::GET},
            {"HEAD", request_method::HEAD},
            {"POST", request_method::POST},
            {"PUT", request_method::PUT},
            {"DELETE", request_method::DELETE},
            {"CONNECT", request_method::CONNECT},
            {"OPTIONS", request_method::OPTIONS},
            {"TRACE", request_method::TRACE},
            {"PATCH", request_method::PATCH}
        };

        return local_map[method];
    }

// protected member functions
    const int teapot::create_socket(
        sock* listening_socket IN OUT,
        const int& domain,
        const int& type,
        const int& protocol) const
    {
        *listening_socket = socket(domain, type, protocol);
        if (*listening_socket < 0) {
            perror("teapot::create_socket() > socket");
            return EXIT_FAILURE;
        }

        if (reuse_address == 1) {
            if (setsockopt(
                    *listening_socket,
                    SOL_SOCKET,
                    SO_REUSEADDR,
                    &reuse_address,
                    sizeof(int)) < 0) {
                perror("teapot::create_socket() > setsockopt (reuseaddr)");
                return EXIT_FAILURE;
            }
        }

        return EXIT_SUCCESS;
    }
}

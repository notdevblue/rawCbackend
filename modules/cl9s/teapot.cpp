#include "teapot.h"
#include <signal.h>

#if CONSOLE_LOG
#include <iostream>
#endif

namespace cl9s
{
    void sigpipe_handler(int sigtype) {
    }

    teapot::teapot(
        const bool& prevent_socket_creation,
        const int& reuse_address) : reuse_address(reuse_address)
    {

        request_method_from_string = {
            {std::string("GET"), request_method::GET},
            {std::string("HEAD"), request_method::HEAD},
            {std::string("POST"), request_method::POST},
            {std::string("PUT"), request_method::PUT},
            {std::string("DELETE"), request_method::DELETE},
            {std::string("CONNECT"), request_method::CONNECT},
            {std::string("OPTIONS"), request_method::OPTIONS},
            {std::string("TRACE"), request_method::TRACE},
            {std::string("PATCH"), request_method::PATCH}};

        signal(SIGPIPE, sigpipe_handler);
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
        ssize_t res;

        res = ::send(client_socket, response, response_size, 0);

        if (res < 0) {
            perror("teapot_server::send() > write");
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
            printf("len: %ld\nRequest:\n%s\n", received, buffer);
#endif
        } while (received > 0 && buffer[received] > 0);

        if (received == 0) {
            return 1; // close, same as EXIT_FAILURE
        }

        if (received < 0) {
            perror("teapot_server::receive() > read");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const int teapot::close_socket(sock& socket IN, const int& how) {
        if (socket == 0) {
            return EXIT_SUCCESS;
        }

        if (shutdown(socket, how) < 0) {
            // maybe wait?
            perror("teapot::close_socket() > shutdown");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

// public member functions
    const request_method& teapot::str_to_request_method(const std::string& method) {
        return request_method_from_string[method];
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

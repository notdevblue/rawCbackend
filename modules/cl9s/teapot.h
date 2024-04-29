#pragma once

#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> // if compiling on windows, uninstall windows
#include <cstdio>
#include <cstdlib>
#include <unordered_map>

#define IN
#define OUT
#define STOP_EXCEPTION -1
#define STOP_SCHEDULED 0

namespace cl9s
{
    typedef int sock;

    enum class request_method : int {
        GET = 0,
        HEAD,
        POST,
        PUT,
        DELETE,
        CONNECT,
        OPTIONS,
        TRACE,
        PATCH,
        END_OF_ENUM
    };

    class teapot
    {
    public:
        teapot(
            const bool& prevent_socket_creation = false,
            const int& reuse_address = 1) : reuse_address(reuse_address)
        {

            request_method_from_string = {
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
        }

        virtual ~teapot() {
        }
    
    public:
        // Summary:
        //  send data to client
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        static const int send(
            const sock& client_socket,
            const char* response,
            const size_t& response_size)
        {
            if (write(client_socket, response, response_size) < 0) {
                perror("teapot_server::send() > write");
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }

        // Summary:
        //  receives data from client
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILIRE on Fail
        static const int receive(
            const sock& client_socket,
            char* buffer,
            const size_t& buffer_size)
        {
            ssize_t received;

            do {
                received = read(client_socket, buffer, buffer_size);
            } while (received > 0 && buffer[received] > 0);

            if (received < 0) {
                perror("teapot_server::receive() > read");
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }

        // Summary:
        //  closes connection with client
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        static const int close_connection(
            const sock& client_socket,
            const int& how = SHUT_RDWR)
        {
            if (shutdown(client_socket, how)) {
                perror("teapot_server::close_connection() > shutdown");
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }

    public:
        // Summary:
        //  Closes socket.
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        const int close_socket(sock& socket IN, const int& how = SHUT_RDWR) {
            if (socket == 0) {
                return EXIT_SUCCESS;
            }

            if (shutdown(socket, how) < 0) {
                perror("teapot::close_socket() > shutdown");
                return EXIT_FAILURE;
            }

            socket = 0;

            return EXIT_SUCCESS;
        }

        const request_method& str_to_request_method(char* method) {
            return request_method_from_string[method];
        }

        virtual void stop(const int& how, const char* errmsg = "") = 0;

    protected:
        // Summary:
        //  Creates socket.
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        const int create_socket(
            sock* listening_socket IN OUT,
            const int& domain = PF_INET,
            const int& type = SOCK_STREAM,
            const int& protocol = IPPROTO_TCP
        ) const {
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
                        sizeof(int)) < 0)
                {
                    perror("teapot::create_socket() > setsockopt");
                    return EXIT_FAILURE;
                }
            }

            return EXIT_SUCCESS;
        }

    private:
        std::unordered_map<const char*, request_method> request_method_from_string;

    private:
        int reuse_address;
    };
};

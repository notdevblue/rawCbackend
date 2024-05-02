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
        teapot(const bool& prevent_socket_creation = false, const int& reuse_address = 1);
        virtual ~teapot();

    public:
        /// @brief Sends data to client
        /// @param client_socket socket to send data to
        /// @return EXIT_SUCCESS on Success, EXIT_FAILURE on Fail
        static const int send(
            const sock& client_socket,
            const char* response,
            const size_t& response_size);

        /// @brief Receives data from client
        /// @param client_socket socket to receive data from
        /// @return 0 on Success, 1 on Close
        static const int receive(
            const sock& client_socket,
            char* buffer,
            const size_t& buffer_size);

        /// @brief Closes socket.
        /// @param socket socket to close
        /// @return EXIT_SUCCESS on Success, EXIT_FAILURE on Fail
        static const int close_socket(sock& socket IN, const int& how = SHUT_RDWR);
        
    public:
        /// @brief Changes GET, POST, etc to request_method::GET, request_method::POST, etc.
        /// @param method request method
        /// @return method as request_method enum class
        const request_method& str_to_request_method(char* method);

        virtual void stop(const int& how, const char* errmsg = "") = 0;

    protected:
        /// @brief Creates socket.
        /// @param listening_socket new socket object
        /// @return EXIT_SUCCESS on Success, EXIT_FAILURE on Fail
        const int create_socket(
            sock* listening_socket IN OUT,
            const int& domain = PF_INET,
            const int& type = SOCK_STREAM,
            const int& protocol = IPPROTO_TCP) const;

    private:
        std::unordered_map<const char*, request_method> request_method_from_string;

    private:
        int reuse_address;
    };
};

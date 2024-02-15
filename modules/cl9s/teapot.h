#pragma once

#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> // if compiling on windows, uninstall windows


namespace cl9s
{
    typedef int sock;

    class teapot
    {
    public:
        teapot(const bool& prevent_socket_creation = false, const int& reuse_address = 1) {
            this->reuse_address = reuse_address;

            if (!prevent_socket_creation) {
                create_socket();
            }
        }

        virtual ~teapot() {
            close_socket();
        }
        // should draw structure tomorrow or some other day

        // what to do untimately,
        // backend is backend.
        // client application (let's go rust fuck yeah mega project) can connect via http protocol (or https idk)
        // web browser can connecto via http protocol (or https idk)
        // GET 0.0.0.0/?locat=<location> => backend sends today's weather as plain string
        // GET 0.0.0.0/browser?locat=<location> => backend sends today's weather as pretty html, js, css

        // willing that this class is used by server and client application,
        // so no client or server specific codes.
        // create derived class to write client or server specific codes.
    public:
        // Summary:
        //  Closes socket.
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        const int close_socket(const int& how = SHUT_RDWR) const {
            if (m_socket != 0) {
                return EXIT_SUCCESS;
            }

            if (shutdown(m_socket, how) < 0) {
                perror("teapot::close_socket() > shutdown");
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }

    protected:
        // Summary:
        //  Creates socket.
        //
        // Returns:
        //  EXIT_SUCCESS on Success
        //  EXIT_FAILURE on Fail
        const int create_socket(
            const int& domain = PF_INET,
            const int& type = SOCK_STREAM,
            const int& protocol = IPPROTO_TCP
        ) {
            m_socket = socket(domain, type, protocol);
            if (m_socket < 0) {
                perror("teapot::create_socket() > socket");
                return EXIT_FAILURE;
            }

            if (reuse_address == 1) {
                if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_address, sizeof(int)) < 0) {
                    perror("teapot::create_socket() > setsockopt");
                    return EXIT_FAILURE;
                }
            }

            return EXIT_SUCCESS;
        }
        
    protected:
        sock m_socket;

    private:
        int reuse_address;
    };
};

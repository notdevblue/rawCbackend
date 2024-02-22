#include "teapot_server.h"


namespace cl9s
{
    const int teapot_server::listen_connection(const int& address_family = AF_INET) const {
        sockaddr_in serverAddr;
        {
            serverAddr.sin_family = address_family;
            serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            serverAddr.sin_port = htons(m_port);
        };

        if (bind(m_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("teapot_server::listen_connection() > bind");
            return EXIT_FAILURE;
        }

        if (listen(m_socket, SOMAXCONN) < 0) {
            perror("teapot_server::listen_connection() > listen");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
}

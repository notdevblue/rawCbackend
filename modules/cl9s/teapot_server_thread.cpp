#include <cstdlib>
#include <thread>
#include <unistd.h>
#include <stdio.h>

#include "teapot_server.h"

#define DEAFULT_FAILSAFE_COUNT 25

namespace cl9s
{
    const std::shared_ptr<std::thread>& teapot_server::handle_client_connection() {
        m_connection_thread = std::make_shared<std::thread>(handle_client_thread, this);

        return m_connection_thread;
    }

    void teapot_server::stop(const int& how, const char* errmsg) {
        this->m_bKeepAcceptConnection = false;

        if (how == STOP_EXCEPTION) {
            printf("Called stop with exception\n");
            perror(errmsg);
            exit(EXIT_FAILURE);
            return;
        }
    }

    void teapot_server::handle_client_thread() {
        unsigned char listeningFailsafe = 0;
        sock listening_socket;

        while (this->m_bKeepAcceptConnection) {
            if (this->create_socket(&listening_socket) < 0) {
                ++listeningFailsafe;

                if (listeningFailsafe >= DEAFULT_FAILSAFE_COUNT) {
                    this->stop(STOP_EXCEPTION);
                }
                continue;
            }
        }
    }
}

#include <cstdlib>
#include <thread>
#include <unistd.h>
#include <stdio.h>

#include "teapot_server.h"

namespace cl9s
{
    const std::thread& teapot_server::handle_client_connection() {


        return m_connection_thread;
    }
}

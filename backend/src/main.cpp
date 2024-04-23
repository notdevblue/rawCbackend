#include <iostream>
#include <string>

#include <cl9s/teapot_server.h>
#include <memory.h>

const int PORT = 30000;

#include <string.h>

int main() {
    cl9s::teapot_server serv = cl9s::teapot_server(PORT);

    serv.route(cl9s::request_method::GET, "/", [](auto req, auto res) {
        
    });

    auto handle_thread = serv.handle_client_connection();

    return 0;
}

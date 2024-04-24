#include <iostream>
#include <string>
#include <cl9s/teapot_server.h>

const int PORT = 30000;

int main() {
    cl9s::teapot_server serv = cl9s::teapot_server(PORT);

    serv.route(cl9s::request_method::GET, "/", [](const cl9s::request& req, const cl9s::response& res) {
        char result[] =
            "HTTP/1.1 200 OK\n"
            "Content-Length: 22\n"
            "Content-Type: text/plain; charset=utf-8\n"
            "\n"
            "Install Gentoo Always!\n";

        res.send_response(result, sizeof(result));
    });

    auto handle_thread = serv.handle_client_connection();

    return 0;
}

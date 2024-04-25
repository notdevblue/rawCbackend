#include <iostream>
#include <string>
#include <cl9s/teapot_server.h>
#include <cl9s/statuscode.h>

const int PORT = 30000;

int main() {
    cl9s::teapot_server serv = cl9s::teapot_server(PORT);

    serv.route(cl9s::request_method::GET, "/", [](const cl9s::request::req& req, const cl9s::response::res& res) {
        res.send("Install Gentoo and Remove Windows");
    });

    serv.route(cl9s::request_method::GET, "/stop", [&serv](auto req, auto res) {
        res.send("Server stopping.");

        serv.stop(STOP_SCHEDULED);
    });

    auto handle_thread = serv.handle_client_connection();

    return 0;
}

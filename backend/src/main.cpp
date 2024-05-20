#include <iostream>
#include <string>
#include <cl9s/teapot_server.h>
#include <cl9s/statuscode.h>
#include <memory>

const int PORT = 30000;

int main() {
    using namespace cl9s;

    teapot_server serv = teapot_server(PORT);

    serv.route(request_method::GET, "/", [](const request& req, const response& res) {
        res.send(content::text("Install Gentoo and Remove Windows"));
    });

    serv.route(request_method::GET, "/index", [](const request& req, const response& res) {
        res.send(content::text(std::make_unique<file>(file("/index.html"))));
    });

    serv.route(request_method::POST, "/login", [](const request& req, const response& res) {
        std::cout << "ID: " << req.get_querystring("id") << std::endl;
        std::cout << "hello: " << req.get_querystring("hello") << std::endl;

        res.send(content::text("Success!"));
    });

    auto handle_thread = serv.handle_client_connection();

    return 0;
}

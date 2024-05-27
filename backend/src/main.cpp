#include <iostream>
#include <string>
#include <cl9s/teapot_server.h>
#include <cl9s/statuscode.h>
#include <memory>
#include <csignal>

const int PORT = 30000;
std::shared_ptr<cl9s::teapot_server> g_serv; // bad

void graceful_shutdown(int sig) {
    std::signal(SIGTERM, SIG_DFL);
    std::signal(SIGINT, SIG_DFL);

    printf("Server shutting down...\n");
    g_serv->stop(STOP_SCHEDULED);
}

int main() {
    using namespace cl9s;

    g_serv = std::make_shared<teapot_server>(PORT);

    g_serv->route(request_method::GET, "/", [](const request& req, const response& res) {
        res.send(content::text("Install Gentoo and Remove Windows"));
    });

    g_serv->route(request_method::GET, "/index", [](const request& req, const response& res) {
        res.send(content::text(std::make_unique<file>(file("/index.html"))));
    });

    g_serv->route(request_method::POST, "/login", [](const request& req, const response& res) {
        const std::string* str_id = req.get_querystring("id");

        std::cout << "ID: " << (str_id == nullptr ? "NULL" : *str_id) << std::endl;

        res.send(content::text("Success!"));
    });

    std::signal(SIGTERM, graceful_shutdown);
    std::signal(SIGINT, graceful_shutdown);

    g_serv->handle_client_connection();
    return 0;
}

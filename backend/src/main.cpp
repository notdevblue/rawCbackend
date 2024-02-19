#include <iostream>

#include <cl9s/teapot_server.h>

const int PORT = 30000;

int main() {
    cl9s::teapot_server serv = cl9s::teapot_server(30000);

    std::cout << "server listening..." << std::endl;
    serv.listen_connection();
    cl9s::sock client_socket = serv.accept_client();

    std::cout << "client connected." << std::endl;

    char buffer[4096];
    serv.receive(buffer, sizeof(buffer));

    std::cout << "Received:\n" << buffer << "\nEnd of message" << std::endl;

    char response[] =
    "HTTP/1.1 200 OK\n"
    "Content-Length: 22\n"
    "Content-Type: text/plain; charset=utf-8\n"
    "\n"
    "Install Gentoo Always!\n"
    ;

    std::cout << "Sending:\n" << response << "\nEnd of message" << std::endl;

    serv.send(response, sizeof(response));

    return 0;
}


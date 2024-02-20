#include <iostream>

#include <cl9s/teapot_server.h>
#include <memory.h>

const int PORT = 30000;

int main() {
    cl9s::teapot_server serv = cl9s::teapot_server(30000);
    char buffer[4096];
    char response[] =
    "HTTP/1.1 200 OK\n"
    "Content-Length: 22\n"
    "Content-Type: text/plain; charset=utf-8\n"
    "\n"
    "Install Gentoo Always!\n"
    ;

    while (true)
    {
        std::cout << "server listening..." << std::endl;
        serv.listen_connection();

        cl9s::sock client_socket = serv.accept_client();
        std::cout << "client connected." << std::endl;

        serv.receive(buffer, sizeof(buffer));
        serv.send(response, sizeof(response));

        serv.close_connection();
        serv.close_socket();

        serv.handle_client_connection();
        memset(buffer, 0, sizeof(buffer));
    }


    // favicon 요청을 함!
    // std::cout << "server listening..." << std::endl;
    // serv.listen_connection();

    // client_socket = serv.accept_client();
    // std::cout << "client connected." << std::endl;

    // memset(buffer, 0, sizeof(buffer));

    // serv.receive(buffer, sizeof(buffer));
    // std::cout << "recv\n" << buffer << '\n';
    // serv.send(response, sizeof(response));
    // std::cout << "send\n";

    // serv.close_connection();
    // serv.close_socket();

    
    return 0;
}

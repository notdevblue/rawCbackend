#include <iostream>

#include <cl9s/teapot.h>

const int PORT = 30000;

int main() {
    int serverSocket;
    serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    const int reuseAddr = 1;

    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(int)) < 0) {
        perror("stsockopt");
        exit(EXIT_FAILURE);
    }
    
    sockaddr_in serverAddr;
    {
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(PORT);
    }

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    std::cout << "Listening..." << std::endl;
    if (listen(serverSocket, SOMAXCONN) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int clientSocket;
    sockaddr_in clientAddr;
    unsigned int clientLength;

    clientLength = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLength);
    std::cout << "Connected!" << std::endl;

    char buffer[4096];
    read(clientSocket, buffer, sizeof(buffer));

    std::cout << "Received:\n" << buffer << "\nEnd of message" << std::endl;

    char response[] =
    "HTTP/1.1 200 OK\n"
    "Content-Length: 22\n"
    "Content-Type: text/plain; charset=utf-8\n"
    "\n"
    "Install Gentoo Always!\n"
    ;

    std::cout << "Sending:\n" << response << "\nEnd of message" << std::endl;

    write(clientSocket, response, sizeof(response));

    // int msg_size;

    // do {
    //     msg_size = read(clientSocket, buffer, sizeof(buffer));
    // } while(msg_size > 0);
    // std::cout << buffer << std::endl;


    shutdown(serverSocket, SHUT_RDWR);

    return 0;
}


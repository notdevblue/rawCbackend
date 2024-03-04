#include <iostream>
#include <string>

#include <cl9s/teapot_server.h>
#include <memory.h>

const int PORT = 30000;

int main() {
    cl9s::teapot_server serv = cl9s::teapot_server(PORT);


    // TODO
    /*
    serv.get("/path1", [request, response] {});
    serv.post("/path2", [request, response] {});
    */

    auto handle_thread = serv.handle_client_connection();
    return 0;
}

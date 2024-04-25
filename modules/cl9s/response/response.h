#pragma once

#include "../teapot.h"
#include <string>


namespace cl9s::response
{
    class res {
    public:
        res(sock& client_socket);
        ~res();

        const sock& get_client() const;

        void send(const std::string& data, const int& statuscode) const;

    private:
    private:
        sock& m_client_socket;
    };
};
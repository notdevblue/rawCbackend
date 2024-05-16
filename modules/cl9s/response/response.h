#pragma once

#include "../statuscode.h"
#include "../teapot.h"
#include "../content/content_types.h"
#include <string>


namespace cl9s
{
    class response {
    public:
        response(sock& client_socket);
        ~response();

        const sock& get_client() const;

        void send(const content::contents& data, const status& code = status::OK) const;

    private:
        const std::string get_date_string() const;

    private:
        sock& m_client_socket;
    };
}

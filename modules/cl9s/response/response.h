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

        /// @brief sends content
        /// @param data content to send
        /// @param code response status code
        /// @return EXIT_SUCCESS on Success, EXIT_FAILURE on Fail
        const int send(const content::contents& data, const status& code = status::OK) const;

    private:
        sock& m_client_socket;
    };
}

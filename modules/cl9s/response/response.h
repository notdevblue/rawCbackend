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

        inline const int send_404() const {
            return send(content::text("Not Found."), status::NOT_FOUND);
        }

        inline const int send_400() const {
            return send(content::text("Bad Request."), status::BAD_REQUEST);
        }

    private:
        sock& m_client_socket;
    };
}

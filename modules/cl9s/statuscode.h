#pragma once

namespace cl9s
{
    enum class status : unsigned short {
        OK = 200,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        NOT_FOUND = 404,
        CONFLICT = 409,
        IM_A_TEAPOT = 418,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
    };

    inline const char* create_status_header(const status& status) {
        switch (status)
        {
        case status::OK:                return "HTTP/1.1 200 OK";
        case status::BAD_REQUEST:       return "HTTP/1.1 400 Bad Request";
        case status::UNAUTHORIZED:      return "HTTP/1.1 401 Unauthorized";
        case status::NOT_FOUND:         return "HTTP/1.1 404 Not Found";
        case status::CONFLICT:          return "HTTP/1.1 409 Conflict";
        case status::IM_A_TEAPOT:       return "HTTP/1.1 418 I'm a teapot";
        case status::NOT_IMPLEMENTED:   return "HTTP/1.1 501 Not Implemented";
        default:                        return "HTTP/1.1 500 Internal Server Error";
        }
    }
};
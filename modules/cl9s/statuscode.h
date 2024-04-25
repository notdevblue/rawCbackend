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

    const char* create_status_header(const status& status) {
        switch (status)
        {
        case status::OK:                    return "200 OK";
        case status::BAD_REQUEST:           return "400 Bad Request";
        case status::UNAUTHORIZED:          return "401 Unauthorized";
        case status::NOT_FOUND:             return "404 Not Found";
        case status::CONFLICT:              return "409 Conflict";
        case status::IM_A_TEAPOT:           return "418 I'm a teapot";
        case status::INTERNAL_SERVER_ERROR: return "500 Internal Server Error";
        case status::NOT_IMPLEMENTED:       return "501 Not Implemented";
        }
    }
};

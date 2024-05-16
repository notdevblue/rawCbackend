#pragma once
#include "../teapot.h"
#include <string>

namespace cl9s
{
    class request {
    public:
        request();
        request(const request& other) = delete;
        ~request();

    public:
        const std::string& get_querystring() const;
        const std::string& get_location() const;
        const std::string& get_content() const;
        const request_method& get_method() const;

        const bool set(const std::string& buffer);

    private:
        const bool parse_header(const std::string& header);
        const int parse_body(const std::string& body);

    private:
        std::string m_querystring;
        std::string m_location;
        std::string m_content;
        request_method m_method;
    };
};

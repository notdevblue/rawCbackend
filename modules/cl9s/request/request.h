#pragma once
#include "../teapot.h"
#include <string>
#include <map>

#ifdef REQ_DEBUG
#include <iostream>
#endif

namespace cl9s
{
    class request {
    public:
        request();
        request(const request& other) = delete;
        ~request();

    public:
        const std::string& get_querystring(const std::string& key) const;
        const std::string& get_location() const;
        const std::string& get_content() const;
        const request_method& get_method() const;

        /// @brief initializes requset class
        /// @param buffer 
        /// @return EXIT_SUCCESS on Success, EXIT_FAILURE on Fail
        const int set(const std::string& buffer);

    private:
        const int parse_header(const std::string& header);
        const int parse_body(const std::string& body);

        void parse_querystring(const std::string& querystring);

    private:
        std::map<const std::string, std::string> m_querystring;
        std::string m_location;
        std::string m_content;
        request_method m_method;
    };
};

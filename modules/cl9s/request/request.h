#pragma once
#include "../teapot.h"
#include <string>
#include <map>

#ifdef REQ_DEBUG
#include <iostream>
#endif

namespace cl9s
{
    #define BODY_FOLLOWED_AFTER 2

    class request {
    public:
        request();
        request(const request&) = delete;
        ~request();

    public:
        const std::string& get_querystring(const char* key) const;
        const std::string& get_header_content(const char* key) const;
        const std::string& get_location() const;
        const std::string& get_content() const;
        const request_method& get_method() const;

        /// @brief initializes requset class
        /// @param buffer 
        /// @return EXIT_SUCCESS on Success, EXIT_FAILURE on Fail
        const int set(const std::string& buffer);

        /// @brief initializes body
        /// @param body
        /// @return EXIT_SUCCESS on Success, EXIT_FAILURE on Fail
        const int parse_body(const std::string& body);

        inline const bool& is_body_needs_parsing() const noexcept {
            return m_body_followed;
        }


    private:
        const int parse_header(const std::string& header);
        const int parse_www_form_urlencoded(const std::string& body);
        const int parse_form_data(const std::string& body, const std::string& boundary);
        void parse_querystring_like(const std::string& source, std::map<const std::string, std::string>& to);

    private:
        std::map<const std::string, std::string> m_querystring;
        std::map<const std::string, std::string> m_header_contents;
        std::string m_location;
        std::string m_content;
        request_method m_method;
        bool m_body_followed;
    };
};

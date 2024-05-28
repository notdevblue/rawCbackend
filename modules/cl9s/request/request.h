#pragma once
#include "../teapot.h"
#include <string>
#include <map>

namespace cl9s
{
    class request {
    public:
        request();
        request(const request&) = delete;
        ~request();

    public:
        /// @brief gets querystring data
        /// @param key key
        /// @return nullptr when null, else data
        const std::string* get_querystring(const char* key) const;

        /// @brief gets header content
        /// @param key key
        /// @return nullptr when null, else data
        const std::string* get_header_content(const char* key) const;

        const std::string& get_location() const;
        const std::string& get_content() const;
        const std::string* get_content(const std::string& key) const;
        const request_method& get_method() const;

        /// @brief initializes requset class
        /// @param buffer 
        /// @return EXIT_SUCCESS on Success, EXIT_FAILURE on Fail
        const int set(const std::string& buffer);

        /// @brief initializes body
        /// @param body
        void parse_body(const std::string& body);

        inline const bool& is_body_needs_parsing() const noexcept {
            return m_body_followed;
        }


    private:
        const int parse_header(const std::string& header);
        void parse_www_form_urlencoded(const std::string& body);
        void parse_form_data(const std::string& body, const std::string& boundary);
        void parse_querystring_like(const std::string& source, std::map<const std::string, std::string>& to);

    private:
        std::map<const std::string, std::string> m_querystring;
        std::map<const std::string, std::string> m_header_contents;
        std::map<const std::string, std::string> m_body;

        std::string m_location;
        std::string m_content;
        request_method m_method;
        bool m_body_followed;
    };
};

#pragma once
#include "../teapot.h"
#include <string>

namespace cl9s::request
{
    class req {
    public:
        req(const std::string location, const std::string querystring, const std::string header);
        req(const req& other) = delete;
        ~req();

    public:
        const std::string& get_querystring() const;
        const std::string& get_location() const;
        const std::string& get_content() const;

    private:
        std::string m_querystring;
        std::string m_location;
        std::string m_content;
    };
};

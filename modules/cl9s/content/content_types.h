#pragma once
#include <file/filereader.h>
#include <string>
#include <memory>

namespace cl9s::content
{
    class contents
    {
    public:
        virtual const std::string get_header_str() const = 0;
        virtual const int length() const = 0;
        virtual const char* get_data() const = 0;

    private:
    };


    class text : public contents
    {
    public:
        text(const std::string& data);
        text(std::unique_ptr<file> f);
        text(const text&) = delete;

    public:
        const std::string get_header_str() const override;
        const int length() const override;
        const char* get_data() const override;

    private:
        std::string m_type;
        std::string m_subtype;
        std::string m_data;
        std::unique_ptr<file> m_file;
    };
}

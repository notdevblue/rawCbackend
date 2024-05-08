#pragma once
#include <file/filereader.h>
#include <string>

namespace cl9s::content
{
    class contents
    {
    public:
        virtual const std::string& get_type() const = 0;

    private:
    };


    class text : public contents
    {
    public:
        text(const std::string& data);
        text(file& f);
        text(const text&) = delete;

    public:
        const std::string& get_type() const override;

    private:
        std::string m_type;
        std::string m_subtype;
        std::string m_data;
        file m_file;
    };
}

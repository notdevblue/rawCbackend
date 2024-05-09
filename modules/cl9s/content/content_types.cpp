#include "content_types.h"
#include <memory>

namespace cl9s::content
{
    text::text(const std::string& data) : m_type("text"), m_subtype("plain"), m_data(data) {
    }

    text::text(std::unique_ptr<file> f) : m_type("text"), m_file(std::move(f)) {
        const char* ext = m_file->extension();
        m_subtype = ext == "js" ? "javascript" : ext;
        m_data = m_file->read();
    }

    const std::string text::get_header_str() const {
        return std::string("Content-Length: ")
            .append(std::to_string(length()))
            .append("\nContent-type: ")
            .append(m_type)
            .append("/")
            .append(m_subtype)
            .append("; charset=utf-8\n")
            .c_str();
    }

    const int text::length() const {
        return m_data.length();
    }

    const char* text::get_data() const {
        return m_data.c_str();
    }
}

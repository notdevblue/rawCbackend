#include "content_types.h"


namespace cl9s::content
{
    text::text(const std::string& data) : m_type("text"), m_subtype("plain"), m_data(data) {
    }

    text::text(file& f) : m_type("text"), m_file(std::move(f)) {
        const char* ext = f.extension();
        m_subtype = ext == "js" ? "javascript" : ext;
    }

    const std::string& text::get_type() const {
        return m_type + "/" + m_subtype;
    }
}

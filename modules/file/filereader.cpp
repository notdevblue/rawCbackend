#include "filereader.h"
#include <stdio.h>
#include <string>
#include <string.h>

file::file() {
}

file::file(const char* path) {
    m_data = nullptr;
    m_path = std::string(path);
}

const char* file::read() {
    if (m_data != nullptr) {        
        free(m_data);
    }

    m_fp = fopen((std::string(PUBLICS) + m_path).c_str(), "r");
    if (!m_fp) {
        perror((std::string("Failed to open ") + m_path).c_str());
        return nullptr;
    }

    fseek(m_fp, 0, SEEK_END);
    size_t size = ftell(m_fp);
    fseek(m_fp, 0, SEEK_SET);

    m_data = (char*)malloc(sizeof(char) * (size + 1));
    memset(m_data, 0, sizeof(char) * (size + 1));

    fread_unlocked(m_data, sizeof(char), size, m_fp);
    fclose(m_fp);

    return m_data;
}

const char* file::extension(){
    return m_path.substr(m_path.find_last_of('.')).c_str();
}

file::~file() {
    if (m_data != nullptr) {
        free(m_data);
    }
}
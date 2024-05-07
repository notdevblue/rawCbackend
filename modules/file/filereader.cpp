#include "filereader.h"
#include <stdio.h>
#include <string>
#include <string.h>

file::file(const char* path) {
    m_data = nullptr;
    m_path = (char*)malloc(sizeof(path));
    strcpy(m_path, path);
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

    fread_unlocked(m_data, size, size, m_fp);
    fclose(m_fp);

    return m_data;
}

file::~file() {
    if (m_data != nullptr) {
        free(m_data);
    }

    free(m_path);
}

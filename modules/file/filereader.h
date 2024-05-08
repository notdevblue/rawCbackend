#pragma once

#include <string>
#include <stdio.h>

class file {
public:
    file();
    file(const char* path);
    const char* read();
    const char* extension();
    ~file();

private:
    std::string m_path;
    FILE* m_fp;
    char* m_data;
};

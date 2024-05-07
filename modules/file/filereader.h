#pragma once

#include <string>
#include <stdio.h>

class file {
public:
    file(const char* path);
    const char* read();
    ~file();

private:
    char* m_path;
    FILE* m_fp;
    char* m_data;
};

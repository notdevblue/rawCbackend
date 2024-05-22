#pragma once

#include <stdlib.h>
#include <cstring>

class strdup_raii {
public:
    strdup_raii(const char* _source) {
        assign(_source);
    }

    strdup_raii() {
        str = nullptr;
    }

    ~strdup_raii() {
        if (str != nullptr) {
            free(str);
        }
    }

    void assign(const char* _source) {
        if (_source == nullptr) {
            throw "Null pointer exception";
        }

        str = strdup(_source);
    }

    char*& get() {
        return str;
    }

private:
    char* str;
};

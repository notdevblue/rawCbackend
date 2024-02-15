#pragma once

#include "teapot.h"

namespace cl9s
{
    class teapot_client : public teapot
    {
    public:
        teapot_client(const int& reuse_address = 1) : teapot(false, reuse_address) {

        }

        virtual ~teapot_client() {

        }

        
    };
};

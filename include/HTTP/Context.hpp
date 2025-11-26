#pragma once

#include <string>

#include "HTTP/Method.hpp"

namespace http
{
    struct Context
    {
        Method method;
        std::string path;
    };
}
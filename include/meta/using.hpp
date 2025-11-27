#pragma once

#include <functional>

namespace http
{
    struct Context;
}

namespace hsc
{
    using MiddlewareCallback = std::function<void(http::Context &)>;
}
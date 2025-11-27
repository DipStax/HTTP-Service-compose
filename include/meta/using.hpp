#pragma once

namespace http
{
    struct Context;
}

namespace hsc
{
    using MiddlewareCallback = std::function<void(http::Context &)>;
}
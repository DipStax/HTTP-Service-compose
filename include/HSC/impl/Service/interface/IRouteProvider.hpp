#pragma once

#include "HTTP/Context.hpp"
#include "HTTP/Response.hpp"

namespace hsc::impl
{
    class IRouteProvider
    {
        public:
            virtual ~IRouteProvider() = default;

            virtual http::Response dispatch(http::Context &_ctx) = 0;
    };
}
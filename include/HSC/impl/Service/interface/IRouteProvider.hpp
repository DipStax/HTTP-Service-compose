#pragma once

#include "HTTP/Context.hpp"

namespace hsc::impl
{
    class IRouteProvider
    {
        public:
            virtual ~IRouteProvider() = default;

            virtual void dispatch(http::Context &_ctx) = 0;
    };
}
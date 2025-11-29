#pragma once

#include "HTTP/Context.hpp"
#include "HTTP/Response.hpp"

namespace hsc::impl
{
    /// @brief Base interface for providing route
    class IRouteProvider
    {
        public:
            virtual ~IRouteProvider() = default;

            /// @brief Dispatch the HTTP context to the targeted route
            /// @param _ctx HTTP context to dispatch
            /// @return The HTTP response of the route
            virtual http::Response dispatch(http::Context &_ctx) = 0;
    };
}
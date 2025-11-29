#pragma once

#include <functional>

#include "HSC/impl/Service/interface/IRouteProvider.hpp"

#include "HTTP/Context.hpp"
#include "HTTP/Response.hpp"

#include "meta/using.hpp"

namespace hsc::impl
{
    /// @brief Internal middleware to dispatch HTTP event to the targeted route
    class DispatchRoute
    {
        public:
            DispatchRoute(MiddlewareCallback _next, std::shared_ptr<IRouteProvider> _route_provider);
            ~DispatchRoute() = default;

            http::Response invoke(http::Context &_context);

        private:
            std::shared_ptr<IRouteProvider> m_route_provider;   ///< Route provider singleton service
    };
}
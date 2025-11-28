#pragma once

#include <functional>

#include "HSC/impl/Service/interface/IRouteProvider.hpp"

#include "HTTP/Context.hpp"
#include "HTTP/Response.hpp"

#include "meta/using.hpp"

namespace hsc::impl
{
    class DispatchRoute
    {
        public:
            DispatchRoute(MiddlewareCallback _next, std::shared_ptr<IRouteProvider> _route_provider)
                : m_route_provider(_route_provider)
            {
                std::ignore = _next;
            }
            ~DispatchRoute() = default;

            http::Response invoke(http::Context &_context)
            {
                return m_route_provider->dispatch(_context);
            }

        private:
            std::shared_ptr<IRouteProvider> m_route_provider;
    };
}
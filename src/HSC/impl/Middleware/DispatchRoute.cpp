#include "HSC/impl/Middleware/DispatchRoute.hpp"

namespace hsc::impl
{
    DispatchRoute::DispatchRoute(MiddlewareCallback _next, std::shared_ptr<IRouteProvider> _route_provider)
        : m_route_provider(_route_provider)
    {
        std::ignore = _next;
    }

    http::Response DispatchRoute::invoke(http::Context &_context)
    {
        return m_route_provider->dispatch(_context);
    }
}
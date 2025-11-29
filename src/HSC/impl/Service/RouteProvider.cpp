#include "HSC/impl/Service/RouteProvider.hpp"

namespace hsc::impl
{
    RouteProvider::RouteProvider(
        std::shared_ptr<IServiceProvider> _service_provider,
        std::vector<std::unique_ptr<ARegisteredRoute>> _routes
    )
        : m_service_provider(_service_provider), m_routes(std::move(_routes))
    {
    }

    http::Response RouteProvider::dispatch(http::Context &_ctx)
    {
        for (const std::unique_ptr<ARegisteredRoute> &_route : m_routes) {
            if (_route->match(_ctx.method, _ctx.path))
                return _route->run(m_service_provider);
        }
        return http::Response{};
    }
}
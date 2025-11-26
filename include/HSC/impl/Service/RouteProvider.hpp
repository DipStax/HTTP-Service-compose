#pragma once

#include "HSC/impl/Service/ServiceProvider.hpp"

#include "HTTP/Context.hpp"

#include "HSC/Registery/Route.hpp"

namespace hsc::impl
{
    class IRouteProvider
    {
        public:
            virtual ~IRouteProvider() = default;

            virtual void dispatch(http::Context &_ctx) = 0;
    };

    class RouteProvider : public IRouteProvider
    {
        public:
            RouteProvider(std::shared_ptr<IServiceProvider> _service_provider, std::vector<std::unique_ptr<ARegisteredRoute>> _routes)
                : m_service_provider(_service_provider), m_routes(std::move(_routes))
            {
            }

            void dispatch(http::Context &_ctx)
            {
                for (const std::unique_ptr<ARegisteredRoute> &_route : m_routes) {
                    if (_route->match(_ctx.method, _ctx.path))
                        _route->run(m_service_provider);
                }
            }

        private:
            std::shared_ptr<IServiceProvider> m_service_provider;

            std::vector<std::unique_ptr<ARegisteredRoute>> m_routes;
    };
}
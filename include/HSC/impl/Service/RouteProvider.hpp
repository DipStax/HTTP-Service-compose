#pragma once

#include "HSC/impl/Service/interface/IRouteProvider.hpp"
#include "HSC/impl/Service/interface/IServiceProvider.hpp"

#include "HSC/Registery/Route.hpp"

namespace hsc::impl
{
    class RouteProvider : public IRouteProvider
    {
        public:
            RouteProvider(std::shared_ptr<IServiceProvider> _service_provider, std::vector<std::unique_ptr<ARegisteredRoute>> _routes);

            void dispatch(http::Context &_ctx);

        private:
            std::shared_ptr<IServiceProvider> m_service_provider;

            std::vector<std::unique_ptr<ARegisteredRoute>> m_routes;
    };
}
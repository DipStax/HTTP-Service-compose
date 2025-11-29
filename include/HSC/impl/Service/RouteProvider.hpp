#pragma once

#include "HSC/impl/Service/interface/IRouteProvider.hpp"
#include "HSC/impl/Service/interface/AServiceProvider.hpp"

#include "HSC/Registery/Route.hpp"

namespace hsc::impl
{
    class RouteProvider : public IRouteProvider
    {
        public:
            RouteProvider(std::shared_ptr<AServiceProvider> _service_provider, std::vector<std::unique_ptr<ARegisteredRoute>> _routes);

            http::Response dispatch(http::Context &_ctx);

        private:
            std::shared_ptr<AServiceProvider> m_service_provider;       ///< Service provider service

            std::vector<std::unique_ptr<ARegisteredRoute>> m_routes;    ///< Route available in the application
    };
}
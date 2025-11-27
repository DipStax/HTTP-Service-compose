#include "HSC/impl/Service/RouteProvider.hpp"
#include "HSC/impl/Service/ServiceProvider.hpp"

#include "HSC/ServiceBuilder.hpp"
#include "HSC/ServiceCollection.hpp"

namespace hsc
{
    ServiceCollection ServiceBuilder::build()
    {
        addSingletonInternal<impl::IRouteProvider, impl::RouteProvider, ^^::hsc::impl>(std::move(m_registered_routes));
        // addSingleton<impl::IServiceProvider, impl::ServiceProvider>(std::move(m_services));
        m_services.push_back(std::make_shared<Service<impl::IServiceProvider, impl::ServiceProvider>>(ServiceType::Singleton, [] (std::shared_ptr<impl::IServiceProvider> &_service_provider, ScopedContainer &_scope_container) {
            std::ignore = _service_provider;
            std::ignore = _scope_container;

            return nullptr;
        }));

        std::shared_ptr<impl::ServiceProvider> service_provider = std::make_shared<impl::ServiceProvider>(std::move(m_services));

        service_provider->buildSingleton();
        return ServiceCollection{service_provider};
    }
}
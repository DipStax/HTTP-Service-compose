#include "HSC/impl/Service/RouteProvider.hpp"

#include "HSC/ServiceBuilder.hpp"
#include "HSC/ServiceCollection.hpp"

namespace hsc
{
    ServiceCollection ServiceBuilder::build()
    {
        addSingleton<impl::IServiceProvider, impl::ServiceProvider>(std::move(m_services));
        addSingleton<impl::IRouteProvider, impl::RouteProvider>(std::move(m_registered_routes));

        return ServiceCollection{*this};
    }
}
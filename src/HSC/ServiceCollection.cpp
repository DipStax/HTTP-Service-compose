#include "HSC/impl/Middleware/DispatchRoute.hpp"

#include "HSC/ServiceCollection.hpp"
#include "HSC/ServiceBuilder.hpp"

namespace hsc
{
    ServiceCollection::ServiceCollection(std::shared_ptr<impl::IServiceProvider> _service_provider)
        : m_service_provider(_service_provider)
    {
    }

    void ServiceCollection::dispatch(http::Method _method, const std::string &_path)
    {
        addMiddleware<impl::DispatchRoute>();
    }
}
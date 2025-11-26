#include "HSC/ServiceCollection.hpp"
#include "HSC/ServiceBuilder.hpp"

namespace hsc
{
    ServiceCollection::ServiceCollection(ServiceBuilder &_service_builder)
    {
    }

    void ServiceCollection::dispatch(http::Method _method, const std::string &_path)
    {
    }
}
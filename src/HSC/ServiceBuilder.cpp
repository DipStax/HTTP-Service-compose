#include "HSC/ServiceBuilder.hpp"
#include "HSC/ServiceCollection.hpp"

namespace hsc
{
    ServiceCollection ServiceBuilder::build()
    {
        return ServiceCollection{*this};
    }
}
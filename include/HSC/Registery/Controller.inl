#include "HSC/Registery/Controller.hpp"
#include "HSC/ServiceContainer.hpp"

namespace hsc
{
    template<IsController ControllerType>
    RegisteredController<ControllerType>::RegisteredController(ControllerCtor _ctor)
        : m_ctor(_ctor)
    {
    }

    template<IsController ControllerType>
    std::shared_ptr<ControllerType> RegisteredController<ControllerType>::create(ServiceContainer &_service_container)
    {
        return m_ctor(_service_container);
    }
}
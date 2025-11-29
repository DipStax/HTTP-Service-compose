#include "HSC/impl/Service/interface/AServiceProvider.hpp"
#include "HSC/Registery/Controller.hpp"

namespace hsc
{
    template<IsController ControllerType>
    RegisteredController<ControllerType>::RegisteredController(ControllerCtor _ctor)
        : m_ctor(_ctor)
    {
    }

    template<IsController ControllerType>
    std::shared_ptr<ControllerType> RegisteredController<ControllerType>::create(std::shared_ptr<impl::AServiceProvider> &_service_provider)
    {
        return m_ctor(_service_provider);
    }
}
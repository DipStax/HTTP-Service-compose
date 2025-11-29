#include "HSC/impl/Service/interface/AServiceProvider.hpp"
#include "HSC/Registery/Service.hpp"

namespace hsc
{
    template<IsServiceInterface Interface>
    AServiceWrapper<Interface>::AServiceWrapper(ServiceType _type, std::string_view _interface, std::string_view _implementation)
        : AService(_type, _interface), m_implementation(_implementation)
    {
    }

    template<IsServiceInterface Interface>
    std::any AServiceWrapper<Interface>::build(std::shared_ptr<impl::AServiceProvider> &_service_provider)
    {
        return create(_service_provider);
    }

    template<IsServiceInterface Interface, IsServiceImplementation Implementation>
    Service<Interface, Implementation>::Service(ServiceType _type, Ctor _ctor)
        : AServiceWrapper<Interface>(_type,
            std::meta::identifier_of(std::meta::dealias(^^Interface)),
            std::meta::identifier_of(std::meta::dealias(^^Implementation))
        ),
        m_ctor(_ctor)
    {
    }

    template<IsServiceInterface Interface, IsServiceImplementation Implementation>
    std::shared_ptr<Interface> Service<Interface, Implementation>::create(std::shared_ptr<impl::AServiceProvider> &_service_provider)
    {
        return m_ctor(_service_provider);
    }
}
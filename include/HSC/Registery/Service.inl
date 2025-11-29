#include "HSC/impl/Service/interface/IServiceProvider.hpp"
#include "HSC/Registery/Service.hpp"

namespace hsc
{
    template<IsInterface Interface>
    AServiceWrapper<Interface>::AServiceWrapper(ServiceType _type, std::string_view _interface, std::string_view _implementation)
        : AService(_type, _interface), m_implementation(_implementation)
    {
    }

    template<IsInterface Interface>
    std::any AServiceWrapper<Interface>::build(std::shared_ptr<impl::IServiceProvider> &_service_provider, ScopedContainer &_scoped_container)
    {
        return create(_service_provider, _scoped_container);
    }

    template<IsInterface Interface, IsServiceImplementation Implementation>
    Service<Interface, Implementation>::Service(ServiceType _type, Ctor _ctor)
        : AServiceWrapper<Interface>(_type,
            std::meta::identifier_of(std::meta::dealias(^^Interface)),
            std::meta::identifier_of(std::meta::dealias(^^Implementation))
        ),
        m_ctor(_ctor)
    {
    }

    template<IsInterface Interface, IsServiceImplementation Implementation>
    std::shared_ptr<Interface> Service<Interface, Implementation>::create(std::shared_ptr<impl::IServiceProvider> &_service_provider, ScopedContainer &_scoped_container)
    {
        return m_ctor(_service_provider, _scoped_container);
    }
}
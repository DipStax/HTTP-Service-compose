#include "HSC/Registery/Service.hpp"

#include "HSC/ServiceContainer.hpp"
#include "HSC/ScopedContainer.hpp"

namespace hsc
{
    template<IsInterface Interface>
    AServiceWrapper<Interface>::AServiceWrapper(ServiceType _type, std::string_view _interface, std::string_view _implementation)
        : AService(_type, _interface), m_implementation(_implementation)
    {
    }

    template<IsInterface Interface>
    std::any AServiceWrapper<Interface>::build(ServiceContainer &_service_container, ScopedContainer &_scoped_container)
    {
        return create(_service_container, _scoped_container);
    }

    template<IsInterface Interface, IsServiceImplementation Implementation>
    Service<Interface, Implementation>::Service(ServiceType _type, Ctor _ctor)
        : AServiceWrapper<Interface>(_type,
            std::meta::identifier_of(std::meta::dealias(^^Interface)),
            std::meta::identifier_of(std::meta::dealias(^^ImplementationType))
        ),
        m_ctor(_ctor)
    {
    }

    template<IsInterface Interface, IsServiceImplementation Implementation>
    std::shared_ptr<Interface> Service<Interface, Implementation>::create(ServiceContainer &_service_container, ScopedContainer &_scoped_container)
    {
        return m_ctor(_service_container, _scoped_container);
    }
}
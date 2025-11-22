#pragma once

#include <functional>
#include <meta>

enum ServiceType
{
    Singleton,
    Transcient,
    Scoped,
};

struct IService
{

    IService(ServiceType _type, std::string_view _interface)
        : m_type(_type), m_interface(_interface)
    {
    }
    virtual ~IService() = default;

    const ServiceType m_type;
    const std::string_view m_interface;
};

template<class Interface>
struct IServiceWrapper : IService
{
    using InterfaceType = Interface;

    IServiceWrapper(ServiceType _type, std::string_view _interface, std::string_view _implementation)
        : IService(_type, _interface), m_implementation(_implementation)
    {
    }
    virtual ~IServiceWrapper() = default;

    virtual std::shared_ptr<InterfaceType> create() = 0;

    const std::string_view m_implementation;
};

template<class Interface, class Implementation>
struct Service : IServiceWrapper<Interface>
{
    using ImplementationType = Implementation;
    using Ctor = std::function<std::shared_ptr<Interface>()>;

    Service(ServiceType _type, Ctor _ctor)
        : IServiceWrapper<Interface>(_type,
            std::meta::identifier_of(std::meta::dealias(^^Interface)),
            std::meta::identifier_of(std::meta::dealias(^^ImplementationType))
        ),
        m_ctor(_ctor)
    {
    }

    std::shared_ptr<Interface> create() override
    {
        return m_ctor();
    }

    Ctor m_ctor;
};
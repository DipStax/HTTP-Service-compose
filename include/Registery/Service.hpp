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

    IService(ServiceType _type, std::string_view _identifier)
        : m_type(_type), m_identifier(_identifier)
    {
    }
    virtual ~IService() = default;

    const ServiceType m_type;
    const std::string_view m_identifier;
};

template<class Interface, class Implementation>
struct Service : IService
{
    using InterfaceType = Interface;
    using ImplementationType = Implementation;
    using Ctor = std::function<std::shared_ptr<InterfaceType>()>;

    Service(ServiceType _type, Ctor _ctor)
        : IService(_type, std::meta::identifier_of(std::meta::dealias(^^InterfaceType))), m_ctor(_ctor)
    {
    }

    Ctor m_ctor;
};
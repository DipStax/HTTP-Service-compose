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
    using Ctor = std::function<void()>;

    IService(ServiceType _type, Ctor _ctor, std::string_view _identifier)
        : m_ctor(_ctor), m_type(_type), m_identifier(_identifier)
    {
    }
    virtual ~IService() = default;

    const ServiceType m_type;
    const std::string_view m_identifier;
    Ctor m_ctor;
};

template<class Interface, class Implementation>
struct Service : IService
{
    Service(ServiceType _type, IService::Ctor _ctor)
        : IService(_type, _ctor, std::meta::identifier_of(^^Interface))
    {
    }

    using interface = Interface;
    using implementation = Implementation;
};
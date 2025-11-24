#include "HSC/Registery/Service.hpp"

namespace hsc
{
    AService::AService(ServiceType _type, std::string_view _interface)
        : m_type(_type), m_interface(_interface)
    {
    }

    ServiceType AService::getType() const
    {
        return m_type;
    }

    const std::string_view &AService::getInterface() const
    {
        return m_interface;
    }
}
#include "HSC/ScopedContainer.hpp"

namespace hsc
{
    bool ScopedContainer::contains(const std::string_view &_interface) const
    {
        return m_services.contains(_interface);
    }

    std::any ScopedContainer::getService(const std::string_view &_interface) const
    {
        return m_services.at(_interface);
    }

    void ScopedContainer::registerService(const std::string_view &_interface, std::any _implementation)
    {
        m_services[_interface] = _implementation;
    }
}
#include "ServiceContainer.hpp"

ServiceContainer(
    std::unordered_map<std::string_view, std::shared_ptr<AService>> _singleton_services,
    std::unordered_map<std::string_view, std::shared_ptr<AService>> _scoped_services,
    std::unordered_map<std::string_view, std::shared_ptr<AService>> _transcient_services
)
    : m_singleton_services(std::move(_singleton_services)),
    m_scoped_services(std::move(_scoped_services)),
    m_transcient_services(std::move(_transcient_services))
{
    buildSingletonService();
}

std::any getSingletonService(const std::string_view &_interface) const
{
    return m_singleton_services.at(_interface);
}

const std::shared_ptr<AService> &getServiceInfo(const std::string_view &_interface) const
{
    if (m_singleton_services.contains(_interface))
        return m_singleton_services.at(_interface);
    if (m_scoped_services.contains(_interface))
        return m_scoped_services.at(_interface);
    if (m_transcient_services.contains(_interface))
        return m_transcient_services.at(_interface);
    throw "Unable to find the interface in the container";
}

void buildSingletonService()
{
    for (const auto [_interface, _service] : m_singleton_services) {
        if (!m_singleton_services_implementation.contains(_interface)) {
            ScopedContainer scoped_container{};

            m_singleton_services_implementation[_interface] = _service->build(*this, scoped_container);
        }
    }
}
#include "HSC/impl/Service/RouteProvider.hpp"

namespace hsc::impl
{
    ServiceProvider::ServiceProvider(std::vector<std::shared_ptr<AService>> _services)
        : m_singleton_services(
            _services
                | std::views::filter([] (const std::shared_ptr<AService> &_service) { return _service->getType() == ServiceType::Singleton; })
                | std::views::transform([](const std::shared_ptr<AService>& _service) { return std::pair<std::string_view, std::shared_ptr<AService>>{_service->getInterface(), _service}; })
                | std::ranges::to<std::map>()
        ),
        m_scoped_services(
            _services
                | std::views::filter([] (const std::shared_ptr<AService> &_service) { return _service->getType() == ServiceType::Scoped; })
                | std::views::transform([](const std::shared_ptr<AService>& _service) { return std::pair<std::string_view, std::shared_ptr<AService>>{_service->getInterface(), _service}; })
                | std::ranges::to<std::map>()
        ),
        m_transcient_services(
            _services
                | std::views::filter([] (const std::shared_ptr<AService> &_service) { return _service->getType() == ServiceType::Transient; })
                | std::views::transform([](const std::shared_ptr<AService>& _service) { return std::pair<std::string_view, std::shared_ptr<AService>>{_service->getInterface(), _service}; })
                | std::ranges::to<std::map>()
        )
    {
    }

    std::any ServiceProvider::getSingletonService(const std::string_view &_interface) const
    {
        return m_singleton_services.at(_interface);
    }

    const std::shared_ptr<AService> &ServiceProvider::getServiceInfo(const std::string_view &_interface) const
    {
        if (m_singleton_services.contains(_interface))
            return m_singleton_services.at(_interface);
        if (m_scoped_services.contains(_interface))
            return m_scoped_services.at(_interface);
        if (m_transcient_services.contains(_interface))
            return m_transcient_services.at(_interface);
        throw "Unable to find the interface in the container";
    }

    void ServiceProvider::buildSingletonService()
    {
        for (const auto &[_interface, _service] : m_singleton_services) {
            if (!m_singleton_services_implementation.contains(_interface)) {
                ScopedContainer scoped_container{};

                m_singleton_services_implementation[_interface] = _service->build(*this, scoped_container);
            }
        }
    }
}
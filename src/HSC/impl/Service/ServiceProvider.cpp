#include <ranges>
#include <print>

#include "HSC/impl/Service/ServiceProvider.hpp"
#include "HSC/ScopedContainer.hpp"

namespace hsc::impl
{
    ServiceProvider::ServiceProvider(std::vector<std::shared_ptr<AService>> _services)
        : m_singleton_services(
            _services
                | std::views::filter([] (const std::shared_ptr<AService> &_service) {
                        return _service->getType() == ServiceType::Singleton;
                    })
                | std::views::transform([](const std::shared_ptr<AService> &_service) {
                        return std::pair<std::string_view, std::shared_ptr<AService>>{_service->getInterface(), _service};
                    })
                | std::ranges::to<std::map>()
        ),
        m_scoped_services(
            _services
                | std::views::filter([] (const std::shared_ptr<AService> &_service) {
                        return _service->getType() == ServiceType::Scoped;
                    })
                | std::views::transform([](const std::shared_ptr<AService> &_service) {
                        return std::pair<std::string_view, std::shared_ptr<AService>>{_service->getInterface(), _service};
                    })
                | std::ranges::to<std::map>()
        ),
        m_transcient_services(
            _services
                | std::views::filter([] (const std::shared_ptr<AService> &_service) {
                        return _service->getType() == ServiceType::Transient;
                    })
                | std::views::transform([](const std::shared_ptr<AService> &_service) {
                        return std::pair<std::string_view, std::shared_ptr<AService>>{_service->getInterface(), _service};
                    })
                | std::ranges::to<std::map>()
        )
    {
    }

    void ServiceProvider::buildSingleton()
    {
        constexpr std::string_view identifier = std::meta::identifier_of(^^IServiceProvider);
        std::shared_ptr<IServiceProvider> this_provider = shared_from_this();

        m_singleton_services_implementation[identifier] = this_provider;
        for (const auto &[_interface, _service] : m_singleton_services) {
            if (!m_singleton_services_implementation.contains(_interface)) {
                ScopedContainer scoped_container{};

                m_singleton_services_implementation[_interface] = _service->build(this_provider, scoped_container);
            }
        }
    }

    bool ServiceProvider::contains(ServiceType _type, const std::string_view &_interface) const
    {
        switch (_type) {
            case ServiceType::Singleton: return m_singleton_services.contains(_interface);
            case ServiceType::Scoped: return m_scoped_services.contains(_interface);
            case ServiceType::Transient: return m_transcient_services.contains(_interface);
            default: throw "Service type not supported";
        }
    }

    bool ServiceProvider::contains(const std::string_view &_interface) const
    {
        return contains(ServiceType::Singleton, _interface)
            || contains(ServiceType::Scoped, _interface)
            || contains(ServiceType::Transient, _interface);
    }

    ServiceType ServiceProvider::getServiceType(const std::string_view &_interface) const
    {
        if (contains(ServiceType::Singleton, _interface))
            return ServiceType::Singleton;
        if (contains(ServiceType::Scoped, _interface))
            return ServiceType::Scoped;
        if (contains(ServiceType::Transient, _interface))
            return ServiceType::Transient;
        throw "Unable to find the interface";
    }

    std::any ServiceProvider::getSingletonService(const std::string_view &_interface) const
    {
        return m_singleton_services_implementation.at(_interface);
    }

    const std::shared_ptr<AService> &ServiceProvider::getServiceInfo(const std::string_view &_interface) const
    {
        if (contains(ServiceType::Singleton, _interface))
            return m_singleton_services.at(_interface);
        if (contains(ServiceType::Scoped, _interface))
            return m_scoped_services.at(_interface);
        if (contains(ServiceType::Transient, _interface))
            return m_transcient_services.at(_interface);
        throw "Unable to find the interface in the container";
    }
}
#include <ranges>
#include <format>
#include <thread>

#include "HSC/Exception/ServiceException.hpp"
#include "HSC/impl/Service/ServiceProvider.hpp"

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
        constexpr std::string_view identifier = std::meta::identifier_of(^^AServiceProvider);
        std::shared_ptr<AServiceProvider> this_provider = shared_from_this();

        m_singleton_services_implementation[identifier] = this_provider;
        for (const auto &[_interface, _service] : m_singleton_services)
            if (!m_singleton_services_implementation.contains(_interface))
                m_singleton_services_implementation[_interface] = _service->build(this_provider);
    }

    void ServiceProvider::registerScope(const std::string &_scope_id)
    {
        m_scopes[_scope_id] = {};
    }

    void ServiceProvider::unregisterScope(const std::string &_scope_id)
    {
        m_scopes.erase(_scope_id);
    }

    bool ServiceProvider::contains(ServiceType _type, const std::string_view &_interface) const
    {
        switch (_type) {
            case ServiceType::Singleton: return m_singleton_services.contains(_interface);
            case ServiceType::Scoped: return m_scoped_services.contains(_interface);
            case ServiceType::Transient: return m_transcient_services.contains(_interface);
            default: throw ServiceException("Service type not supported", _interface);
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
        throw ServiceException("Unable to find the interface type", _interface);
    }

    std::any ServiceProvider::getSingletonService(const std::string_view &_interface) const
    {
        return m_singleton_services_implementation.at(_interface);
    }

    std::any ServiceProvider::getScopedService(const std::string_view &_interface, const std::string &_scope_id)
    {
        std::map<std::string_view, std::any> &scope = m_scopes.at(_scope_id);

        if (!scope.contains(_interface)) {
            const std::shared_ptr<AService> &service_info = getServiceInfo(_interface);
            std::shared_ptr<AServiceProvider> service_provider = shared_from_this();

            scope[_interface] = service_info->build(service_provider);
        }
        return scope.at(_interface);
    }

    std::any ServiceProvider::getTransientService(const std::string_view &_interface)
    {
        const std::shared_ptr<AService> &service_info = getServiceInfo(_interface);
        std::shared_ptr<AServiceProvider> service_provider = shared_from_this();

        return service_info->build(service_provider);
    }

    const std::shared_ptr<AService> &ServiceProvider::getServiceInfo(const std::string_view &_interface) const
    {
        if (contains(ServiceType::Singleton, _interface))
            return m_singleton_services.at(_interface);
        if (contains(ServiceType::Scoped, _interface))
            return m_scoped_services.at(_interface);
        if (contains(ServiceType::Transient, _interface))
            return m_transcient_services.at(_interface);
        throw ServiceException("Unable to find the interface info", _interface);
    }
}
#include "ServiceCollection.hpp"

ServiceCollection::ServiceCollection(ServiceBuilder &_service_builder)
    : m_registered_routes(std::move(_service_builder.m_registered_routes)),
    m_service_container(
        _service_builder.m_services
            | std::views::filter([] (const std::shared_ptr<AService> &_service) { return _service->getType() == ServiceType::Singleton; })
            | std::views::transform([](const std::shared_ptr<AService>& _service) { return std::pair<std::string_view, std::shared_ptr<AService>>{_service->getInterface(), _service}; })
            | std::ranges::to<std::unordered_map>(),
        _service_builder.m_services
            | std::views::filter([] (const std::shared_ptr<AService> &_service) { return _service->getType() == ServiceType::Scoped; })
            | std::views::transform([](const std::shared_ptr<AService>& _service) { return std::pair<std::string_view, std::shared_ptr<AService>>{_service->getInterface(), _service}; })
            | std::ranges::to<std::unordered_map>(),
        _service_builder.m_services
            | std::views::filter([] (const std::shared_ptr<AService> &_service) { return _service->getType() == ServiceType::Transcient; })
            | std::views::transform([](const std::shared_ptr<AService>& _service) { return std::pair<std::string_view, std::shared_ptr<AService>>{_service->getInterface(), _service}; })
            | std::ranges::to<std::unordered_map>()
    )
{
}

void ServiceCollection::dispatch(http::Method _method, const std::string &_path)
{
    for (const std::unique_ptr<ARegisteredRoute> &_route : m_registered_routes) {
        if (_route->match(_method, _path)) {
            _route->run(m_service_container);
            return;
        }
    }
}
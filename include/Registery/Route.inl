#include "Registery/Route.hpp"

template<class ControllerType>
RegisteredRoute<ControllerType>::RegisteredRoute(const std::string_view _route, http::Method _type, SharedRegisteredControllerType _registered_controller, AttachController _attach)
    : ARegisteredRoute(_route, _type), m_registered_controller(_registered_controller), m_attach(_attach)
{
}

template<class ControllerType>
void RegisteredRoute<ControllerType>::run(ServiceContainer &_service_container)
{
    SharedReferenceControllerType controller = m_registered_controller->create(_service_container);

    m_attach(controller);
}
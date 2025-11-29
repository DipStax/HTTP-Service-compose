#include "HSC/impl/Service/interface/AServiceProvider.hpp"
#include "HSC/Registery/Route.hpp"

namespace hsc
{
    template<IsController ControllerType>
    RegisteredRoute<ControllerType>::RegisteredRoute(const std::string_view _route, http::Method _method, SharedRegisteredControllerType _registered_controller, AttachController _attach)
        : ARegisteredRoute(_route, _method), m_registered_controller(_registered_controller), m_attach(_attach)
    {
    }

    template<IsController ControllerType>
    http::Response RegisteredRoute<ControllerType>::run(std::shared_ptr<impl::AServiceProvider> &_service_provider)
    {
        SharedReferenceControllerType controller = m_registered_controller->create(_service_provider);

        return m_attach(controller);
    }
}
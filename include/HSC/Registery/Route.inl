#include "HSC/impl/Service/interface/IServiceProvider.hpp"
#include "HSC/Registery/Route.hpp"
#include "HSC/ServiceContainer.hpp"

namespace hsc
{
    template<IsController ControllerType>
    RegisteredRoute<ControllerType>::RegisteredRoute(const std::string_view _route, http::Method _method, SharedRegisteredControllerType _registered_controller, AttachController _attach)
        : ARegisteredRoute(_route, _method), m_registered_controller(_registered_controller), m_attach(_attach)
    {
    }

    template<IsController ControllerType>
    void RegisteredRoute<ControllerType>::run(std::shared_ptr<impl::IServiceProvider> &_service_provider)
    {
        SharedReferenceControllerType controller = m_registered_controller->create(_service_provider);

        m_attach(controller);
    }
}
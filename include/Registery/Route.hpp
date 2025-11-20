#pragma once

#include "HTTP/Method.hpp"
#include <string_view>

struct IRegisteredRoute
{
    IRegisteredRoute(const std::string_view _route, http::Method _type)
        : m_route(_route), m_type(_type)
    {
    }
    virtual ~IRegisteredRoute() = default;

    virtual void run() = 0;

    const std::string_view m_route;
    const http::Method m_type;
};

template<class ControllerType>
struct RegisteredRoute : IRegisteredRoute
{
    using ReferenceControllerType = ControllerType;
    using RegisteredControllerType = RegisteredController<ReferenceControllerType>;
    using SharedRegisteredControllerType = std::shared_ptr<RegisteredControllerType>;

    using AttachController = std::function<void(ReferenceControllerType &)>;

    RegisteredRoute(const std::string_view _route, http::Method _type, SharedRegisteredControllerType _registered_controller, AttachController _attach)
        : IRegisteredRoute(_route, _type), m_registered_controller(_registered_controller), m_attach(_attach)
    {
    }

    void run() override
    {
        ReferenceControllerType controller = m_registered_controller->m_ctor();

        m_attach(controller);
    }

    SharedRegisteredControllerType m_registered_controller;
    AttachController m_attach;
};
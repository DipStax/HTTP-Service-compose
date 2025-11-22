#pragma once

#include <string_view>

#include "HTTP/Method.hpp"
#include "Registery/Controller.hpp"

/// @brief Abstraction of a HTTP route
struct ARegisteredRoute
{
    ARegisteredRoute(const std::string_view _route, http::Method _type);
    virtual ~ARegisteredRoute() = default;

    /// @brief Run the implemented route
    virtual void run() = 0;

    /// @brief HTTP route representation
    const std::string_view m_route;
    /// @brief HTTP Method of the route
    const http::Method m_type;
};

/// @brief Implementation of a HTTP route
/// @tparam ControllerType Controller related to the route
template<class ControllerType>
class RegisteredRoute : public ARegisteredRoute
{
    public:
        using ReferenceControllerType = ControllerType;
        using SharedReferenceControllerType = std::shared_ptr<ReferenceControllerType>;
        using RegisteredControllerType = RegisteredController<ReferenceControllerType>;
        using SharedRegisteredControllerType = std::shared_ptr<RegisteredControllerType>;

        using AttachController = std::function<void(SharedReferenceControllerType)>;

        RegisteredRoute(const std::string_view _route, http::Method _type, SharedRegisteredControllerType _registered_controller, AttachController _attach);
        ~RegisteredRoute() = default;

        void run() override;

    private:
        SharedRegisteredControllerType m_registered_controller;
        AttachController m_attach;
};

#include "Registery/Route.inl"
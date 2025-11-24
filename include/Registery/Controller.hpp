#pragma once

#include <functional>

#include "ServiceContainer.hpp"

/// @brief Controller representation
/// @tparam ControllerType Controller implementation type
template<class ControllerType>
class RegisteredController
{
    public:
        using Controller = ControllerType;
        // should be modify by std::move_only_function and use std::unique_ptr as return type
        using ControllerCtor = std::function<std::shared_ptr<Controller>(ServiceContainer &)>;

        RegisteredController(ControllerCtor _ctor);
        ~RegisteredController() = default;

        /// @brief Create the instance of the contoller
        /// @return The instance of the controller
        std::shared_ptr<Controller> create(ServiceContainer &_service_container);

    private:
        ControllerCtor m_ctor;
};

#include "Registery/Controller.inl"
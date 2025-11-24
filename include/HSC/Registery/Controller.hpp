#pragma once

#include <functional>

#include "HSC/ServiceContainer.hpp"

namespace hsc
{
    /// @brief Controller representation
    /// @tparam ControllerType Controller implementation type
    template<class Controller>
    class RegisteredController
    {
        public:
            using ControllerType = Controller;
            // should be modify by std::move_only_function and use std::unique_ptr as return type
            using ControllerCtor = std::function<std::shared_ptr<ControllerType>(ServiceContainer &)>;

            RegisteredController(ControllerCtor _ctor);
            ~RegisteredController() = default;

            /// @brief Create the instance of the contoller
            /// @return The instance of the controller
            std::shared_ptr<ControllerType> create(ServiceContainer &_service_container);

        private:
            ControllerCtor m_ctor;
    };
}

#include "HSC/Registery/Controller.inl"
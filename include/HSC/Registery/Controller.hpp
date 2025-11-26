#pragma once

#include <functional>


#include "meta/concept.hpp"

namespace hsc
{
    namespace impl
    {
        class IServiceProvider;
    }

    /// @brief Controller representation
    /// @tparam ControllerType Controller implementation type
    template<IsController Controller>
    class RegisteredController
    {
        public:
            using ControllerType = Controller;
            // should be modify by std::move_only_function and use std::unique_ptr as return type
            using ControllerCtor = std::function<std::shared_ptr<ControllerType>(std::shared_ptr<impl::IServiceProvider> &)>;

            RegisteredController(ControllerCtor _ctor);
            ~RegisteredController() = default;

            /// @brief Create the instance of the contoller
            /// @return The instance of the controller
            std::shared_ptr<ControllerType> create(std::shared_ptr<impl::IServiceProvider> &_service_provider);

        private:
            ControllerCtor m_ctor;
    };
}

#include "HSC/Registery/Controller.inl"
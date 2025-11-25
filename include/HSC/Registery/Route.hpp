#pragma once

#include <string_view>

#include "HSC/Registery/Controller.hpp"

#include "HTTP/Method.hpp"

#include "meta/concept.hpp"

namespace hsc
{
    /// @brief Abstraction of a HTTP route
    class ARegisteredRoute
    {
        public:
            ARegisteredRoute(const std::string_view _route, http::Method _method);
            virtual ~ARegisteredRoute() = default;

            /// @brief Run the implemented route
            virtual void run(ServiceContainer &_service_container) = 0;

            /// @brief Check if the provided argument match to the route
            /// @param _method Method of the request
            /// @param _path Path of the request
            /// @return True if it's a match, otherwise false
            [[nodiscard]] bool match(http::Method _method, const std::string &_path) const;

        private:
            const std::string_view m_route;     ///< HTTP route representation
            const http::Method m_method;        ///< HTTP Method of the route
    };

    /// @brief Implementation of a HTTP route
    /// @tparam ControllerType Controller related to the route
    template<IsController ControllerType>
    class RegisteredRoute : public ARegisteredRoute
    {
        public:
            using ReferenceControllerType = ControllerType;
            using SharedReferenceControllerType = std::shared_ptr<ReferenceControllerType>;
            using RegisteredControllerType = RegisteredController<ReferenceControllerType>;
            using SharedRegisteredControllerType = std::shared_ptr<RegisteredControllerType>;

            using AttachController = std::function<void(SharedReferenceControllerType)>;

            RegisteredRoute(const std::string_view _route, http::Method _method, SharedRegisteredControllerType _registered_controller, AttachController _attach);
            ~RegisteredRoute() = default;

            void run(ServiceContainer &_service_container) override;

        private:
            SharedRegisteredControllerType m_registered_controller;
            AttachController m_attach;
    };
}

#include "HSC/Registery/Route.inl"
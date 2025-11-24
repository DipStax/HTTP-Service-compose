#pragma once

#include <any>
#include <string_view>
#include <unordered_map>

namespace hsc
{
    /// @brief Container for scope during DI
    class ScopedContainer
    {
        public:
            ScopedContainer() = default;
            ~ScopedContainer() = default;

            /// @brief Check if the interface implementation exist in the scope
            /// @param _interface Interface identifier of the service
            /// @return True if it has been found, otherwise false
            [[nodiscard]] bool contains(const std::string_view &_interface) const;

            /// @brief Get the implementation of the service using it's interface identifier
            /// @param _interface Interface identifier of the service to get
            /// @return The service implementation as a std::shared_ptr<Interface> cast to any
            [[nodiscard]] std::any getService(const std::string_view &_interface) const;

            /// @brief Register a new Scoped service in the scope
            /// @param _interface Interface identifier of the service to register
            /// @param _implementation Implementation of the service to register as a std::shared_ptr<Interface> cast to any
            void registerService(const std::string_view &_interface, std::any _implementation);

        private:
            std::unordered_map<std::string_view, std::any> m_services;  ///< Implementation of the service in the scope
    };
}
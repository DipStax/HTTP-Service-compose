#pragma once

#include <map>

#include "HSC/Registery/Service.hpp"

namespace hsc
{
    /// @brief Container for service creator access and information
    class ServiceContainer
    {
        public:
            ServiceContainer(
                std::map<std::string_view, std::shared_ptr<AService>> _singleton_services,
                std::map<std::string_view, std::shared_ptr<AService>> _scoped_services,
                std::map<std::string_view, std::shared_ptr<AService>> _transcient_services
            );

            /// @brief Get the instance of a singleton service
            /// @param _interface Interface identifier of the singleton service to get
            /// @return The shared_ptr to the implementation of the service as any
            [[nodiscard]] std::any getSingletonService(const std::string_view &_interface) const;

            /// @brief Get service creator information
            /// @param _interface Interface identifier of the service
            /// @return The service creator
            [[nodiscard]] const std::shared_ptr<AService> &getServiceInfo(const std::string_view &_interface) const;

        private:
            void buildSingletonService();

            std::map<std::string_view, std::any> m_singleton_services_implementation;           ///< Map of interface identifier to the shared_ptr implementation of a singleton service as any
            const std::map<std::string_view, std::shared_ptr<AService>> m_singleton_services;   ///< Map of interface identifier to service creator for singleton
            const std::map<std::string_view, std::shared_ptr<AService>> m_scoped_services;      ///< Map of interface identifier to service creator for scoped
            const std::map<std::string_view, std::shared_ptr<AService>> m_transcient_services;  ///< Map of interface identifier to service creator for transient
    };
}
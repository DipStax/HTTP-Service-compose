#pragma once

#include "HSC/Registery/Service.hpp"

namespace hsc
{
    class ServiceContainer
    {
        public:
            ServiceContainer(
                std::unordered_map<std::string_view, std::shared_ptr<AService>> _singleton_services,
                std::unordered_map<std::string_view, std::shared_ptr<AService>> _scoped_services,
                std::unordered_map<std::string_view, std::shared_ptr<AService>> _transcient_services
            );

            [[nodiscard]] std::any getSingletonService(const std::string_view &_interface) const;

            [[nodiscard]] const std::shared_ptr<AService> &getServiceInfo(const std::string_view &_interface) const;

        private:
            void buildSingletonService();

            std::unordered_map<std::string_view, std::any> m_singleton_services_implementation;
            const std::unordered_map<std::string_view, std::shared_ptr<AService>> m_singleton_services;
            const std::unordered_map<std::string_view, std::shared_ptr<AService>> m_scoped_services;
            const std::unordered_map<std::string_view, std::shared_ptr<AService>> m_transcient_services;
    };
}
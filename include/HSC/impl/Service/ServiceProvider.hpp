#pragma once

#include <vector>
#include <map>
#include <memory>

#include "HSC/impl/Service/interface/AServiceProvider.hpp"
#include "HSC/Registery/Service.hpp"

namespace hsc::impl
{
    class ServiceProvider : public AServiceProvider, public std::enable_shared_from_this<ServiceProvider>
    {
        public:
            ServiceProvider(std::vector<std::shared_ptr<AService>> _services);
            ~ServiceProvider() = default;

            void buildSingleton();

            bool contains(ServiceType _type, const std::string_view &_interface) const override;
            bool contains(ServiceType _type, const std::string_view &_interface, const std::string &_scope_id) const override;
            bool contains(const std::string_view &_interface) const override;
            bool contains(const std::string_view &_interface, const std::string &_scope_id) const override;

            ServiceType getServiceType(const std::string_view &_interface) const override;
            ServiceType getServiceType(const std::string_view &_interface, const std::string &_scope_id) const override;

            std::any getSingletonService(const std::string_view &_interface) const override;

            std::any getScopedService(const std::string_view &_interface, const std::string &_scope_id) override;

            std::any getTransientService(const std::string_view &_interface) override;

            const std::shared_ptr<AService> &getServiceInfo(const std::string_view &_interface) const override;
            const std::shared_ptr<AService> &getServiceInfo(const std::string_view &_interface, const std::string &_scope_id) const override;

        private:
            std::map<std::string_view, std::any> m_singleton_services_implementation;           ///< Map of interface identifier to the shared_ptr implementation of a singleton service as any
            const std::map<std::string_view, std::shared_ptr<AService>> m_singleton_services;   ///< Map of interface identifier to service creator for singleton
            const std::map<std::string_view, std::shared_ptr<AService>> m_scoped_services;      ///< Map of interface identifier to service creator for scoped
            const std::map<std::string_view, std::shared_ptr<AService>> m_transcient_services;  ///< Map of interface identifier to service creator for transient
            std::map<std::string, std::map<std::string_view, std::any>> m_scopes{};
    };
}
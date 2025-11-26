#pragma once

#include <vector>
#include <map>
#include <memory>

#include "HSC/impl/Service/interface/IServiceProvider.hpp"
#include "HSC/Registery/Service.hpp"

namespace hsc::impl
{
    class ServiceProvider : public IServiceProvider, std::enable_shared_from_this<ServiceProvider>
    {
        public:
            ServiceProvider(std::vector<std::shared_ptr<AService>> _services);
            ~ServiceProvider() = default;

            std::any getSingletonService(const std::string_view &_interface) const override;

            const std::shared_ptr<AService> &getServiceInfo(const std::string_view &_interface) const override;

        private:
            void buildSingletonService();

            std::map<std::string_view, std::any> m_singleton_services_implementation;           ///< Map of interface identifier to the shared_ptr implementation of a singleton service as any
            const std::map<std::string_view, std::shared_ptr<AService>> m_singleton_services;   ///< Map of interface identifier to service creator for singleton
            const std::map<std::string_view, std::shared_ptr<AService>> m_scoped_services;      ///< Map of interface identifier to service creator for scoped
            const std::map<std::string_view, std::shared_ptr<AService>> m_transcient_services;  ///< Map of interface identifier to service creator for transient
    };
}
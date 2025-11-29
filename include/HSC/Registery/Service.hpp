#pragma once

#include <functional>
#include <meta>
#include <any>

#include "HSC/Registery/ServiceType.hpp"

#include "meta/concept.hpp"

namespace hsc
{
    namespace impl
    {
        class AServiceProvider;
    }

    class ScopedContainer;

    class AService
    {
        public:
            AService(ServiceType _type, std::string_view _interface);
            virtual ~AService() = default;

            /// @brief Build the service as an any
            /// @param _service_container Service container
            /// @return The service implementation
            [[nodiscard]] virtual std::any build(std::shared_ptr<impl::AServiceProvider> &_service_provider) = 0;

            /// @brief Get the service type
            /// @return Service type
            [[nodiscard]] ServiceType getType() const;
            /// @brief Get the interface identifier
            /// @return Service Interface identifier
            [[nodiscard]] const std::string_view &getInterface() const;

        private:
            const ServiceType m_type;               ///< Service's service type
            const std::string_view m_interface;     ///< Interface identifier
    };

    template<IsServiceInterface Interface>
    class AServiceWrapper : public AService
    {
        public:
            using InterfaceType = Interface;

            AServiceWrapper(ServiceType _type, std::string_view _interface, std::string_view _implementation);
            virtual ~AServiceWrapper() = default;

            std::any build(std::shared_ptr<impl::AServiceProvider> &_service_provider) override;

            /// @brief Build the service as an interface
            /// @param _service_container Service container
            /// @return The service implementation
            [[nodiscard]] virtual std::shared_ptr<InterfaceType> create(std::shared_ptr<impl::AServiceProvider> &_service_provider) = 0;

        private:
            const std::string_view m_implementation;    ///< Service implementation identifier
    };

    template<IsServiceInterface Interface, IsServiceImplementation Implementation>
    class Service : public AServiceWrapper<Interface>
    {
        public:
            using ImplementationType = Implementation;
            using Ctor = std::function<std::shared_ptr<Interface>(std::shared_ptr<impl::AServiceProvider> &)>;

            Service(ServiceType _type, Ctor _ctor);
            ~Service() = default;

            std::shared_ptr<Interface> create(std::shared_ptr<impl::AServiceProvider> &_service_provider) override;

        private:
            Ctor m_ctor;    ///< Constructor of the service implementation
    };
}

#include "HSC/Registery/Service.inl"
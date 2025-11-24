#pragma once

#include <functional>
#include <meta>
#include <any>

namespace hsc
{
    class ServiceContainer;
    class ScopedContainer;

    enum ServiceType
    {
        Singleton,
        Transcient,
        Scoped,
    };

    class AService
    {
        public:
            AService(ServiceType _type, std::string_view _interface);
            virtual ~AService() = default;

            /// @brief Build the service as an any
            /// @param _service_container Service container
            /// @param _scoped_container Container of service in the current scope
            /// @return The service implementation
            [[nodiscard]] virtual std::any build(ServiceContainer &_service_container, ScopedContainer &_scoped_container) = 0;

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

    template<class Interface>
    class AServiceWrapper : public AService
    {
        public:
            using InterfaceType = Interface;

            AServiceWrapper(ServiceType _type, std::string_view _interface, std::string_view _implementation);
            virtual ~AServiceWrapper() = default;

            std::any build(ServiceContainer &_service_container, ScopedContainer &_scoped_container) override;

            /// @brief Build the service as an interface
            /// @param _service_container Service container
            /// @param _scoped_container Container of service in the current scope
            /// @return The service implementation
            [[nodiscard]] virtual std::shared_ptr<InterfaceType> create(ServiceContainer &_service_container, ScopedContainer &_scoped_container) = 0;

        private:
            const std::string_view m_implementation;    ///< Service implementation identifier
    };

    template<class Interface, class Implementation>
    class Service : public AServiceWrapper<Interface>
    {
        public:
            using ImplementationType = Implementation;
            using Ctor = std::function<std::shared_ptr<Interface>(ServiceContainer &, ScopedContainer &)>;

            Service(ServiceType _type, Ctor _ctor);
            ~Service() = default;

            std::shared_ptr<Interface> create(ServiceContainer &_service_container, ScopedContainer &_scoped_container) override;

        private:
            Ctor m_ctor;    ///< Constructor of the service implementation
    };
}

#include "HSC/Registery/Service.inl"
#pragma once

#include <type_traits>
#include <tuple>
#include <utility>

#include "HSC/Registery/Controller.hpp"
#include "HSC/Registery/Route.hpp"
#include "HSC/Registery/Service.hpp"
#include "HSC/ServiceCollection.hpp"

#include "HTTP/Route.hpp"

#include "meta/concept.hpp"

#ifndef SERVICE_INTERFACE_NAMESPACE
    #define SERVICE_INTERFACE_NAMESPACE ::
#endif

namespace hsc
{
    class ScopedContainer;

    /// @brief Core service container builder
    class ServiceBuilder
    {
        public:
            /// @brief Add a Singleton service to the pool
            /// @tparam Interface Interface type of the service
            /// @tparam Implementation Implementation type of the service
            /// @tparam ...Args Parameters type of extra arguments
            /// @return this
            template<IsInterface Interface, IsServiceImplementation Implementation, class ...Args>
                requires std::is_base_of_v<Interface, Implementation>
            ServiceBuilder &addSingleton(Args &&..._args);

            /// @brief Add a Scoped service to the pool
            /// @tparam Interface Interface type of the service
            /// @tparam Implementation Implementation type of the service
            /// @tparam ...Args Parameters type of extra arguments
            /// @return this
            template<IsInterface Interface, IsServiceImplementation Implementation, class ...Args>
                requires std::is_base_of_v<Interface, Implementation>
            ServiceBuilder &addScoped(Args &&..._args);

            /// @brief Add a Transient service to the pool
            /// @tparam Interface Interface type of the service
            /// @tparam Implementation Implementation type of the service
            /// @tparam ...Args Parameters type of extra arguments
            /// @param ..._args Extra argument to provide to the service when build
            /// @return this
            template<IsInterface Interface, IsServiceImplementation Implementation, class ...Args>
                requires std::is_base_of_v<Interface, Implementation>
            ServiceBuilder &addTransient(Args &&..._args);

            /// @brief Add all the controller in the namespace provided as template parameter
            /// @tparam Namespace Namespace to lookup for controller
            template<std::meta::info Namespace = ^^::>
            void addController();

            /// @brief Build the service collection and add implementation of native service
            /// @return Service collection with information from this service builder
            ServiceCollection build();

        private:
            friend ServiceCollection;

            template<class T>
            using ServiceCreatorCallback = std::function<std::shared_ptr<T>(std::shared_ptr<impl::IServiceProvider> &, ScopedContainer &)>;

            struct TupleCreator
            {
                template<IsInterface Interface, IsServiceImplementation Implementation, size_t ArgsSize>
                static auto CreateSingletonTuple(std::shared_ptr<impl::IServiceProvider> &_service_provider);

                template<IsInterface Interface, IsServiceImplementation Implementation, size_t ArgsSize>
                static auto CreateScopedTuple(std::shared_ptr<impl::IServiceProvider> &_service_provider, ScopedContainer &_scoped_container);

                template<IsInterface Interface, IsServiceImplementation Implementation, size_t ArgsSize>
                static auto CreateTransientTuple(std::shared_ptr<impl::IServiceProvider> &_service_provider, ScopedContainer &_scoped_container);

                template<IsController Controller>
                static auto CreateControllerTuple(std::shared_ptr<impl::IServiceProvider> &_service_provider, ScopedContainer &_scoped_container);
            };

            template<class T, std::meta::info Func>
            static consteval auto GenerateCallback();

            std::vector<std::shared_ptr<AService>> m_services;                      ///< List of service creator
            std::vector<std::unique_ptr<ARegisteredRoute>> m_registered_routes;     ///< List of registered route
    };
}

#include "HSC/ServiceBuilder.inl"
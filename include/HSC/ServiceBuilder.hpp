#pragma once

#include <type_traits>
#include <tuple>
#include <utility>

#include "HSC/Registery/Controller.hpp"
#include "HSC/Registery/Route.hpp"
#include "HSC/Registery/Service.hpp"
#include "HSC/ServiceCollection.hpp"

#include "HTTP/Route.hpp"

#include "meta/extra.hpp"

#ifndef SERVICE_INTERFACE_NAMESPACE
    #define SERVICE_INTERFACE_NAMESPACE ::
#endif

namespace hsc
{
    /// @brief Core service container builder
    class ServiceBuilder
    {
        public:
            /// @brief Add a Singleton service to the pool
            /// @tparam Interface Interface type of the service
            /// @tparam Implementation Implementation type of the service
            /// @tparam ...Args Parameters type of extra arguments
            /// @return this
            template<class Interface, class Implementation, class ...Args>
                requires std::is_base_of_v<Interface, Implementation>
            ServiceBuilder &addSingleton(Args &&..._args);

            /// @brief Add a Scoped service to the pool
            /// @tparam Interface Interface type of the service
            /// @tparam Implementation Implementation type of the service
            /// @tparam ...Args Parameters type of extra arguments
            /// @return this
            template<class Interface, class Implementation, class ...Args>
                requires std::is_base_of_v<Interface, Implementation>
            ServiceBuilder &addScoped(Args &&..._args);

            /// @brief Add a Transient service to the pool
            /// @tparam Interface Interface type of the service
            /// @tparam Implementation Implementation type of the service
            /// @tparam ...Args Parameters type of extra arguments
            /// @param ..._args Extra argument to provide to the service when build
            /// @return this
            template<class Interface, class Implementation, class ...Args>
                requires std::is_base_of_v<Interface, Implementation>
            ServiceBuilder &addTransient(Args &&..._args);

            /// @brief Add all the controller in the namespace provided as template parameter
            /// @tparam Namespace Namespace to lookup for controller
            template<std::meta::info Namespace = ^^::>
            void addController();

            /// @brief Build the service collection
            /// @return Service collection with information from this service builder
            ServiceCollection build();

        private:
            friend ServiceCollection;

            template<class T, std::meta::info Func>
            static consteval auto GenerateCallback();

            template<size_t ...Is>
            static constexpr auto make_parameters_tuple(auto _fn, std::index_sequence<Is...>);

            std::vector<std::shared_ptr<AService>> m_services;                      ///< List of service creator
            std::vector<std::unique_ptr<ARegisteredRoute>> m_registered_routes;     ///< List of registered route

            template<class Implementation, size_t ArgsSize>
            struct ServiceCtorInfo
            {
                static_assert(meta::extra::count_constructor<^^Implementation>() == 1, "Service should have a unique constructor");

                static constexpr std::meta::info ctor = meta::extra::get_unique_ctor<^^Implementation>();
                static constexpr auto params = define_static_array(std::meta::parameters_of(ctor));
                static constexpr size_t params_size = params.size();
                static constexpr size_t params_service_size = params_size - ArgsSize;

                static consteval std::array<std::string_view, params_service_size> GetParametersTypeName();

                static constexpr std::array<std::string_view, params_service_size> interface_names = GetParametersTypeName();
            };

            template<class Implementation>
            using ControllerCtorInfo = ServiceCtorInfo<Implementation, 0>;
    };
}

#include "HSC/ServiceBuilder.inl"
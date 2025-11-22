#pragma once

#include <type_traits>
#include <tuple>
#include <utility>

#include "HTTP/Route.hpp"
#include "meta/extra.hpp"
#include "Registery/Controller.hpp"
#include "Registery/Route.hpp"
#include "Registery/Service.hpp"
#include "ControllerDiscovery.hpp"

#ifndef SERVICE_NAMESPACE
    #define SERVICE_NAMESPACE ::
#endif

/// @brief Core service container builder
class ServiceBuilder
{
    public:
        /// @brief Add a Singleton service to the pool
        /// @tparam Interface Interface type of the service
        /// @tparam Implementation Implementation type of the service
        /// @tparam ...Args Parameters type of extra argument
        /// @param ..._args Extra argument to provide to the service when build
        /// @return this
        template<class Interface, class Implementation, class ...Args>
            requires std::is_base_of_v<Interface, Implementation>
        ServiceBuilder &addScoped(Args &&..._args);

        /// @brief Add a Singleton service to the pool
        /// @tparam Implementation Implementation type of the service
        /// @tparam ...Args Parameters type of extra argument
        /// @param ..._args Extra argument to provide to the service when build
        /// @return this
        template<class Implementation, class ...Args>
        ServiceBuilder &addScoped(const Args &&..._args);

        /// @brief Add all the controller in the namespace provided as template parameter
        /// @tparam Namespace Namespace to lookup for controller
        template<std::meta::info Namespace = ^^::>
        void addController();

        void dispatch(const std::string _route);

    private:
        std::shared_ptr<IService> getService(const std::string_view _name);

        template<std::meta::info Func>
        static consteval std::tuple<http::Method, std::string_view> extract_route_type();

        static consteval bool has_http_unique_annotation(std::meta::info _func);

        template<std::meta::info Func, std::meta::info Annotation>
        static consteval std::optional<std::meta::info> get_unique_route();

        template<class T, std::meta::info Func>
        static consteval auto GenerateCallback();

        template<size_t ...Is>
        constexpr auto make_parameters_tuple(auto _fn, std::index_sequence<Is...>);

        std::vector<std::shared_ptr<IService>> m_services;
        std::vector<std::unique_ptr<ARegisteredRoute>> m_registered_routes;

        template<class Implementation, size_t ArgsSize>
        struct ServiceCtorInfo
        {
            static_assert(meta::extra::count_constructor<^^Implementation>() == 1, "Service should have a unique constructor");

            static constexpr std::meta::info ctor = meta::extra::get_unique_ctor<^^Implementation>();
            static constexpr auto params = define_static_array(std::meta::parameters_of(ctor));
            static constexpr size_t params_size = params.size();
            static constexpr size_t params_service_size = params_size - ArgsSize;

            template<size_t It = 0>
            static consteval std::array<std::string_view, params_service_size> GetParametersTypeName();

            static constexpr std::array<std::string_view, params_service_size> interface_names = GetParametersTypeName();
        };
};

#include "ServiceBuilder.inl"
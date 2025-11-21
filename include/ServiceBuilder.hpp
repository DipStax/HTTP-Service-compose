#pragma once

#include <type_traits>
#include <tuple>

#include "HTTP/Route.hpp"

#include "Registery/Controller.hpp"
#include "Registery/Route.hpp"
#include "Registery/Service.hpp"

#include "ControllerBuilder.hpp"

#include "meta/ctor_getter.hpp"

struct ServiceBuilder
{
    std::vector<std::shared_ptr<IService>> m_services;
    std::vector<std::unique_ptr<IRegisteredRoute>> m_registered_routes;

    template<size_t ...Is>
    constexpr auto make_parameters_tuple(auto _fn, std::index_sequence<Is...>) {
        return std::make_tuple(_fn(std::integral_constant<size_t, Is>{})...);
    }

    template<class Interface, class Implementation, class ...Args>
        requires std::is_base_of_v<Interface, Implementation>
    ServiceBuilder &addSingleton(const Args &&..._args)
    {
        constexpr std::meta::info service_type = ^^Implementation;

        // move to concept to validate type
        if constexpr (extra::meta::count_constructor<service_type>() != 1)
            throw "Controller should have a unique constructor";
        // if constexpr (extra::meta::count_interface<service_type>() != 1)
        //     throw "Service should have only one interface";

        std::function<std::shared_ptr<Interface>()> factory = [this, ...__args = std::forward<Args>(_args)] () -> std::shared_ptr<Interface> {
            constexpr std::meta::info ctor = extra::meta::get_unique_ctor<service_type>();
            constexpr auto params = define_static_array(std::meta::parameters_of(ctor));
            constexpr size_t params_size = params.size();
            constexpr size_t params_size_without_args = params_size - sizeof...(__args);

            auto tuple = make_parameters_tuple([this] (auto _tindex) {
                constexpr size_t index = decltype(_tindex)::value;
                constexpr std::meta::info param_type = type_of(params[index]);

                using ParameterType = [:param_type:];

                constexpr std::string_view type_name = std::meta::identifier_of(std::meta::dealias(^^ParameterType));

                // constexpr std::meta::info target_type = extra::meta::retreive_type(target_service->m_identifier);
                // constexpr std::meta::info target_interface = extra::meta::get_base_interface<target_type>();

                // using TargetType = [:target_type:];
                // using TargetInterface = [:target_interface:];

                // if constexpr (std::is_same_v<TargetInterface, Interface> || std::is_same_v<TargetType, Implementation>)
                //     throw "Can't instanciate the service recursivly"

                // std::shared_ptr<Service<TargetInterface, TargetType>> real_target_service = std::static_pointer_cast<Service<TargetInterface, TargetType>>();

                // return real_target_service->m_ctor();
            }, std::make_index_sequence<params_size_without_args>{});

            return std::apply([... ___args = std::forward<Args>(__args)] (auto &&..._tuple_arg) {
                return std::make_shared<Implementation>(
                    std::forward<delctype(_tuple_arg)>(_tuple_arg)...,
                    std::forward<Args>(___args)...
                );
            }, tuple);
        };

        std::shared_ptr<Service<Interface, Implementation>> service = std::make_shared<Service<Interface, Implementation>>(ServiceType::Singleton, factory);

        m_services.push_back(std::move(service));
        return *this;
    }

    template<class Implementation, class ...Args>
    ServiceBuilder &addSingleton(const Args &&..._args)
    {
        return addSingleton<Implementation, Implementation>(std::forward<Args>(_args)...);
    }

    template<std::meta::info Namespace>
    void addController()
    {
        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto controllers = ControllerBuilder<Namespace>::controllers_list;

        template for (constexpr auto _controller : controllers) {
            constexpr auto annotate_func = define_static_array(std::meta::members_of(_controller, ctx) | std::views::filter([] (std::meta::info _info) {
                return has_http_unique_annotation(_info)
                    && std::meta::is_function(_info)
                    && !is_operator_function(_info)
                    && has_identifier(_info);
            }));

            using ControllerType = [:_controller:];
            using RegisteredRouteType = RegisteredRoute<ControllerType>;
            using RegisteredControllerType = RegisteredRouteType::RegisteredControllerType;
            using SharedRegisteredControllerType = RegisteredRouteType::SharedRegisteredControllerType;

            SharedRegisteredControllerType controller = std::make_shared<RegisteredControllerType>([this] () {
                std::println("controller builder");
                ControllerType controller{};

                return controller;
            });

            template for (constexpr std::meta::info _func : annotate_func) {
                constexpr std::string_view func_identifier = std::meta::identifier_of(_func);

                constexpr std::tuple<http::Method, std::string_view> route_info = extract_route_type<_func>();
                constexpr http::Method route_type = std::get<http::Method>(route_info);
                constexpr std::string_view route = std::get<std::string_view>(route_info);

                std::println("registering {} => {}", func_identifier, route);

                // ICE if using a lambda on ControllerType mangling
                m_registered_routes.push_back(std::make_unique<RegisteredRouteType>(route, route_type, controller, GenerateCallback<ControllerType, _func>()));
            }
        }

    }

    void dispatch(const std::string _route)
    {
        std::println("dispatching route: {}", _route);
        for (std::unique_ptr<IRegisteredRoute> &_registered_route : m_registered_routes) {
            std::println("testing for route: {}", _registered_route->m_route);
            if (_registered_route->m_route == _route)
                _registered_route->run();
        }
    }

    private:
        template<std::meta::info Func>
        static consteval std::tuple<http::Method, std::string_view> extract_route_type()
        {
            constexpr std::optional<std::meta::info> get_route = get_unique_route(Func, ^^http::Get);
            constexpr std::optional<std::meta::info> post_route = get_unique_route(Func, ^^http::Post);

            if (get_route.has_value())
                return std::make_tuple(http::Method::GET, extract<http::Get>(get_route.value()).m_route);
            else if (post_route.has_value())
                return std::make_tuple(http::Method::POST, extract<http::Get>(post_route.value()).m_route);
            throw "Internal error: unable to determine the type of route";
        }


        static consteval bool has_http_unique_annotation(std::meta::info _info) {
            auto http_get = define_static_array(annotations_of(_info, ^^http::Get));
            auto http_post = define_static_array(annotations_of(_info, ^^http::Post));
            size_t total = http_get.size() + http_post.size();

            if (total > 1)
                throw "HTTP route should be single";
            else if (total == 0)
                return false;
            else
                return true;
        }

        static consteval std::optional<std::meta::info> get_unique_route(std::meta::info _func, std::meta::info _annotation)
        {
            std::vector<std::meta::info> annotations = annotations_of(_func, ^^http::Get);

            switch (annotations.size()) {
                case 0: return std::nullopt;
                case 1: return annotations[0];
                default: throw "HTTP route should be unique for a function";
            }
        }

            template<class T, std::meta::info Func>
        static consteval auto GenerateCallback()
        {
            return [] (T &_controller) {
                std::println("calling using controller");
                _controller.[:Func:]();
            };
        }
};
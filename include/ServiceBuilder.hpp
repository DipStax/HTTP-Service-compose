#pragma once

#include <type_traits>

#include "HTTP/Route.hpp"

#include "Registery/Controller.hpp"
#include "Registery/Route.hpp"
#include "Registery/Service.hpp"

#include "ControllerBuilder.hpp"

struct ServiceBuilder
{
    std::vector<std::unique_ptr<IService>> m_services;
    std::vector<std::unique_ptr<IRegisteredRoute>> m_registered_routes;

    template<class Interface, class Implementation, class ...Args>
        requires std::is_base_of_v<Interface, Implementation>
    ServiceBuilder &addSingleton(const Args &&..._args)
    {
        std::unique_ptr<Service<Interface, Implementation>> service = std::make_unique<Service<Interface, Implementation>>(ServiceType::Singleton, [... args = std::forward<Args>(_args)] () {
            Implementation(std::forward<Args>(args)...);
        });

        m_services.push_back(std::move(service));
        return *this;
    }

    template<class Implementation, class ...Args>
    ServiceBuilder &addSingleton(const Args &&..._args)
    {
        std::unique_ptr<Service<Implementation, Implementation>> service = std::make_unique<Service<Implementation, Implementation>>(ServiceType::Singleton, [... args = std::forward<Args>(_args)] () {
            Implementation(std::forward<Args>(args)...);
        });

        m_services.push_back(std::move(service));
        return *this;
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

            SharedRegisteredControllerType controller = std::make_shared<RegisteredControllerType>([] () {
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
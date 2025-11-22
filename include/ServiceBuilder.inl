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




template<class Interface, class Implementation, class ...Args>
    requires std::is_base_of_v<Interface, Implementation>
ServiceBuilder &ServiceBuilder::addScoped(Args &&..._args)
{
    constexpr std::meta::info service_type = ^^Implementation;

    using ServiceCtorInfoInternal = ServiceCtorInfo<Implementation, sizeof...(Args)>;

    if constexpr (meta::extra::count_constructor<service_type>() != 1)
        static_assert(false, "Controller should have a unique constructor");

    constexpr std::meta::info ctor = meta::extra::get_unique_ctor<service_type>();

    std::function<std::shared_ptr<Interface>()> factory = [this, ...__args = std::forward<Args>(_args)] () mutable -> std::shared_ptr<Interface> {
        auto tuple = make_parameters_tuple([this] (auto index_tag) {
            constexpr size_t i = decltype(index_tag)::value;
            constexpr std::string_view interface_name = ServiceCtorInfoInternal::interface_names[i];

            if constexpr (interface_name == std::string_view{std::meta::identifier_of(^^Interface)})
                throw "Can't instantiate the service recursively";

            std::shared_ptr<IService> service = getService(interface_name);

            if (!service)
                throw "Unable to retrieve the service";

            using TargetInterface = [:meta::extra::retreive_type<std::define_static_string(interface_name), ^^SERVICE_INTERFACE_NAMESPACE>().value():];

            std::shared_ptr<IServiceWrapper<TargetInterface>> real_service = std::static_pointer_cast<IServiceWrapper<TargetInterface>>(service);

            if (!real_service)
                throw "Unable to find the implementation of interface";

            return real_service->create();
        }, std::make_index_sequence<ServiceCtorInfoInternal::params_size - sizeof...(Args)>{});

        return std::apply([&] (auto &&...tuple_args) {
            return std::make_shared<Implementation>(
                std::forward<decltype(tuple_args)>(tuple_args)...,
                std::forward<Args>(__args)...
            );
        },tuple);
    };

    std::shared_ptr<Service<Interface, Implementation>> service = std::make_shared<Service<Interface, Implementation>>(ServiceType::Singleton, factory);
    m_services.push_back(service);
    return *this;
}

template<class Implementation, class ...Args>
ServiceBuilder &ServiceBuilder::addScoped(const Args &&..._args)
{
    return addScoped<Implementation, Implementation>(std::forward<Args>(_args)...);
}

template<std::meta::info Namespace>
void ServiceBuilder::addController()
{
    constexpr auto ctx = std::meta::access_context::current();
    constexpr auto controllers = ControllerDiscovery<Namespace>::controllers_list;

    template for (constexpr auto _controller : controllers) {
        constexpr auto annotate_func = define_static_array(std::meta::members_of(_controller, ctx) | std::views::filter([] (std::meta::info _info) {
            return std::meta::is_function(_info)
                && has_http_unique_annotation(_info)
                && !is_operator_function(_info)
                && has_identifier(_info);
        }));

        using ControllerType = [:_controller:];
        using RegisteredRouteType = RegisteredRoute<ControllerType>;
        using RegisteredControllerType = RegisteredRouteType::RegisteredControllerType;
        using SharedRegisteredControllerType = RegisteredRouteType::SharedRegisteredControllerType;

        SharedRegisteredControllerType controller = std::make_shared<RegisteredControllerType>([this] () -> std::unique_ptr<ControllerType> {
            return std::make_unique<ControllerType>();
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

void ServiceBuilder::dispatch(const std::string _route)
{
    std::println("dispatching route: {}", _route);
    for (std::unique_ptr<ARegisteredRoute> &_registered_route : m_registered_routes) {
        std::println("testing for route: {}", _registered_route->m_route);
        if (_registered_route->m_route == _route)
            _registered_route->run();
    }
}

std::shared_ptr<IService> ServiceBuilder::getService(const std::string_view _name)
{
    for (const std::shared_ptr<IService> &_service : m_services)
        if (_service->m_interface == _name)
            return _service;
    return nullptr;
}

template<std::meta::info Func>
consteval std::tuple<http::Method, std::string_view> ServiceBuilder::extract_route_type()
{
    std::optional<std::meta::info> get_route = get_unique_route<Func, ^^http::Get>();
    std::optional<std::meta::info> post_route = get_unique_route<Func, ^^http::Post>();

    if (get_route.has_value())
        return std::make_tuple(http::Method::GET, extract<http::Get>(get_route.value()).m_route);
    else if (post_route.has_value())
        return std::make_tuple(http::Method::POST, extract<http::Get>(post_route.value()).m_route);
    else
        throw "Internal error: unable to determine the type of route";
}


consteval bool ServiceBuilder::has_http_unique_annotation(std::meta::info _func)
{
    std::vector<std::meta::info> http_get = annotations_of(_func, ^^http::Get);
    std::vector<std::meta::info> http_post = annotations_of(_func, ^^http::Post);
    size_t total = http_get.size() + http_post.size();

    if (total > 1)
        throw "HTTP route should be single";
    else if (total == 0)
        return false;
    else
        return true;
}

template<std::meta::info Func, std::meta::info Annotation>
consteval std::optional<std::meta::info> ServiceBuilder::get_unique_route()
{
    std::vector<std::meta::info> annotations = annotations_of(Func, Annotation);

    switch (annotations.size()) {
        case 0: return std::nullopt;
        case 1: return annotations[0];
        default: throw "HTTP route should be unique for a function";
    }
}

template<class T, std::meta::info Func>
consteval auto ServiceBuilder::GenerateCallback()
{
    return [] (std::shared_ptr<T> _controller) {
        std::println("calling using controller");
        (*_controller).[:Func:]();
    };
}

template<size_t ...Is>
constexpr auto ServiceBuilder::make_parameters_tuple(auto _fn, std::index_sequence<Is...>) {
    return std::make_tuple(_fn(std::integral_constant<size_t, Is>{})...);
}

template<class Implementation, size_t ArgsSize>
template<size_t It>
consteval std::array<std::string_view, ServiceBuilder::ServiceCtorInfo<Implementation, ArgsSize>::params_service_size> ServiceBuilder::ServiceCtorInfo<Implementation, ArgsSize>::GetParametersTypeName()
{
    std::array<std::string_view, params_service_size - It> out{};

    constexpr std::meta::info type_info = type_of(params[It]);

    static_assert(std::meta::has_template_arguments(type_info), "Unable to determine the type parameter");
    static_assert(std::meta::template_of(type_info) == ^^std::shared_ptr, "For DI, parameter should be a shared_ptr");

    constexpr auto template_args = define_static_array(std::meta::template_arguments_of(type_info));
    constexpr std::string_view interface_name = std::meta::identifier_of(std::meta::dealias(template_args[0]));

    out[0] = interface_name;
    if constexpr (params_service_size - (It + 1) != 0) {
        constexpr std::array<std::string_view, params_service_size - (It + 1)> new_out = GetParametersTypeName<It + 1>();

        for (size_t it = 1; it < It + 2; it++)
            out[it] = new_out[it - 1];
    }
    return out;
}
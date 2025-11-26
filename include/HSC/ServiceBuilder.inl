#include "HSC/Exception/ControllerDIException.hpp"
#include "HSC/ControllerDiscovery.hpp"
#include "HSC/ServiceBuilder.hpp"
#include "HSC/ScopedContainer.hpp"

#include "HTTP/Route.hpp"

#include "meta/extra.hpp"
#include "meta/http.hpp"

namespace hsc
{
    template<IsInterface Interface, IsServiceImplementation Implementation, class ...Args>
        requires std::is_base_of_v<Interface, Implementation>
    ServiceBuilder &ServiceBuilder::addSingleton(Args &&..._args)
    {
        constexpr std::string_view interface_identifier = std::meta::identifier_of(^^Interface);
        constexpr std::string_view implementation_identifier = std::meta::identifier_of(^^Implementation);

        using ServiceCtorInfoInternal = ServiceCtorInfo<Implementation, sizeof...(Args)>;

        std::function<std::shared_ptr<Interface>(std::shared_ptr<impl::IServiceProvider> &, ScopedContainer &)> factory = [interface_identifier, implementation_identifier, ...__args = std::forward<Args>(_args)] (std::shared_ptr<impl::IServiceProvider> &_service_provider, ScopedContainer &_scoped_container) mutable -> std::shared_ptr<Interface> {
            std::ignore = _scoped_container;

            auto tuple = make_parameters_tuple([interface_identifier, implementation_identifier, &_service_provider] (auto _index) {
                constexpr size_t i = decltype(_index)::value;
                constexpr std::string_view interface_name = ServiceCtorInfoInternal::interface_names[i];

                static_assert(interface_name != interface_identifier, "Can't instantiate the service recursively");

                constexpr std::optional<std::meta::info> target_interface_opt = meta::extra::retreive_type<std::define_static_string(interface_name), ^^SERVICE_INTERFACE_NAMESPACE>();

                static_assert(target_interface_opt.has_value(), "Unable to find the service interface");

                using TargetInterface = [:target_interface_opt.value():];

                const std::shared_ptr<AService> &service_info = _service_provider->getServiceInfo(interface_name);
                ServiceType service_type = service_info->getType();

                if (service_type == ServiceType::Transient)
                    throw hsc::ServiceDIException("You can't inject a transient in a singleton service", implementation_identifier, interface_identifier, interface_name);
                if (service_type == ServiceType::Scoped)
                    throw hsc::ServiceDIException("You can't inject a scoped in a singleton service", implementation_identifier, interface_identifier, interface_name);

                try {
                    return std::any_cast<std::shared_ptr<TargetInterface>>(_service_provider->getSingletonService(interface_name));
                } catch (std::bad_any_cast _ex) {
                    std::ignore = _ex;

                    throw hsc::ServiceDIException("internal error: singleton cast failed, from singleton service", implementation_identifier, interface_identifier, interface_name);
                }
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

    template<IsInterface Interface, IsServiceImplementation Implementation, class ...Args>
        requires std::is_base_of_v<Interface, Implementation>
    ServiceBuilder &ServiceBuilder::addScoped(Args &&..._args)
    {
        constexpr std::string_view interface_identifier = std::meta::identifier_of(^^Interface);
        constexpr std::string_view implementation_identifier = std::meta::identifier_of(^^Implementation);

        using ServiceCtorInfoInternal = ServiceCtorInfo<Implementation, sizeof...(Args)>;

        std::function<std::shared_ptr<Interface>(std::shared_ptr<impl::IServiceProvider> &, ScopedContainer &)> factory = [interface_identifier, implementation_identifier, ...__args = std::forward<Args>(_args)] (std::shared_ptr<impl::IServiceProvider> &_service_provider, ScopedContainer &_scoped_container) mutable -> std::shared_ptr<Interface> {
            auto tuple = make_parameters_tuple([interface_identifier, implementation_identifier, &_service_provider, &_scoped_container] (auto _index) {
                constexpr size_t i = decltype(_index)::value;
                constexpr std::string_view interface_name = ServiceCtorInfoInternal::interface_names[i];

                static_assert(interface_name != interface_identifier, "Can't instantiate the service recursively");

                constexpr std::optional<std::meta::info> target_interface_opt = meta::extra::retreive_type<std::define_static_string(interface_name), ^^SERVICE_INTERFACE_NAMESPACE>();

                static_assert(target_interface_opt.has_value(), "Unable to find the service interface");

                using TargetInterface = [:target_interface_opt.value():];

                const std::shared_ptr<AService> &service_info = _service_provider->getServiceInfo(interface_name);
                ServiceType service_type = service_info->getType();

                if (service_type == ServiceType::Transient)
                    throw hsc::ServiceDIException("You can't inject a transient in a scoped service", implementation_identifier, interface_identifier, interface_name);
                if (service_type == ServiceType::Singleton) {
                    try {
                        return std::any_cast<std::shared_ptr<TargetInterface>>(_service_provider->getSingletonService(interface_name));
                    } catch (std::bad_any_cast _ex) {
                        std::ignore = _ex;

                        throw hsc::ServiceDIException("internal error: singleton cast failed, from scoped service", implementation_identifier, interface_identifier, interface_name);
                    }
                }
                if (service_type == ServiceType::Scoped && _scoped_container.contains(interface_name)) {
                    try {
                        return std::any_cast<std::shared_ptr<TargetInterface>>(_scoped_container.getService(interface_name));
                    } catch (std::bad_any_cast _ex) {
                        std::ignore = _ex;

                        throw hsc::ServiceDIException("internal error: registered scoped cast failed, from scoped service", implementation_identifier, interface_identifier, interface_name);
                    }
                }

                std::shared_ptr<AServiceWrapper<TargetInterface>> service_wrapper = std::static_pointer_cast<AServiceWrapper<TargetInterface>>(service_info);

                if (!service_wrapper)
                    throw hsc::ServiceDIException("Unable to find the implementation of the interface", implementation_identifier, interface_identifier, interface_name);

                std::shared_ptr<TargetInterface> real_service = nullptr;

                try {
                    real_service = service_wrapper->create(_service_provider, _scoped_container);
                } catch (hsc::ServiceDIException &_ex) {
                    throw hsc::ServiceDIException("service creation failed", implementation_identifier, interface_identifier, interface_name, std::make_unique<hsc::ServiceDIException>(std::move(_ex)));
                }
                _scoped_container.registerService(interface_name, real_service);
                return real_service;
            }, std::make_index_sequence<ServiceCtorInfoInternal::params_size - sizeof...(Args)>{});

            return std::apply([&] (auto &&...tuple_args) {
                return std::make_shared<Implementation>(
                    std::forward<decltype(tuple_args)>(tuple_args)...,
                    std::forward<Args>(__args)...
                );
            },tuple);
        };

        std::shared_ptr<Service<Interface, Implementation>> service = std::make_shared<Service<Interface, Implementation>>(ServiceType::Scoped, factory);
        m_services.push_back(service);
        return *this;
    }

    template<IsInterface Interface, IsServiceImplementation Implementation, class ...Args>
        requires std::is_base_of_v<Interface, Implementation>
    ServiceBuilder &ServiceBuilder::addTransient(Args &&..._args)
    {
        constexpr std::string_view interface_identifier = std::meta::identifier_of(^^Interface);
        constexpr std::string_view implementation_identifier = std::meta::identifier_of(^^Implementation);

        using ServiceCtorInfoInternal = ServiceCtorInfo<Implementation, sizeof...(Args)>;

        std::function<std::shared_ptr<Interface>(std::shared_ptr<impl::IServiceProvider> &, ScopedContainer &)> factory = [interface_identifier, implementation_identifier, ...__args = std::forward<Args>(_args)] (std::shared_ptr<impl::IServiceProvider> &_service_provider, ScopedContainer &_scoped_container) mutable -> std::shared_ptr<Interface> {
            auto tuple = make_parameters_tuple([interface_identifier, implementation_identifier, &_service_provider, &_scoped_container] (auto _index) {
                constexpr size_t i = decltype(_index)::value;
                constexpr std::string_view interface_name = ServiceCtorInfoInternal::interface_names[i];

                static_assert(interface_name != std::string_view{std::meta::identifier_of(^^Interface)}, "Can't instantiate the service recursively");

                constexpr std::optional<std::meta::info> target_interface_opt = meta::extra::retreive_type<std::define_static_string(interface_name), ^^SERVICE_INTERFACE_NAMESPACE>();

                static_assert(target_interface_opt.has_value(), "Unable to find the service interface");

                using TargetInterface = [:target_interface_opt.value():];

                const std::shared_ptr<AService> &service_info = _service_provider->getServiceInfo(interface_name);
                ServiceType service_type = service_info->getType();

                if (service_type == ServiceType::Singleton) {
                    try {
                        return std::any_cast<std::shared_ptr<TargetInterface>>(_service_provider->getSingletonService(interface_name));
                    } catch (std::bad_any_cast _ex) {
                        std::ignore = _ex;

                        throw hsc::ServiceDIException("internal error: singleton cast failed, from transient service", implementation_identifier, interface_identifier, interface_name);
                    }
                }
                if (service_type == ServiceType::Scoped && _scoped_container.contains(interface_name)) {
                    try {
                        return std::any_cast<std::shared_ptr<TargetInterface>>(_scoped_container.getService(interface_name));
                    } catch (std::bad_any_cast _ex) {
                        std::ignore = _ex;

                        throw hsc::ServiceDIException("internal error: registered scoped cast failed, from transient service", implementation_identifier, interface_identifier, interface_name);
                    }
                }

                std::shared_ptr<AServiceWrapper<TargetInterface>> service_wrapper = std::static_pointer_cast<AServiceWrapper<TargetInterface>>(service_info);

                if (!service_wrapper)
                    throw hsc::ServiceDIException("Unable to find the implementation of the interface", implementation_identifier, interface_identifier, interface_name);

                std::shared_ptr<TargetInterface> real_service = nullptr;

                try {
                    real_service = service_wrapper->create(_service_provider, _scoped_container);
                } catch (hsc::ServiceDIException &_ex) {
                    throw hsc::ServiceDIException("service creation failed", implementation_identifier, interface_identifier, interface_name, std::make_unique<hsc::ServiceDIException>(std::move(_ex)));
                }
                if (service_type == ServiceType::Scoped)
                    _scoped_container.registerService(interface_name, real_service);
                return real_service;
            }, std::make_index_sequence<ServiceCtorInfoInternal::params_size - sizeof...(Args)>{});

            return std::apply([&] (auto &&...tuple_args) {
                return std::make_shared<Implementation>(
                    std::forward<decltype(tuple_args)>(tuple_args)...,
                    std::forward<Args>(__args)...
                );
            },tuple);
        };

        std::shared_ptr<Service<Interface, Implementation>> service = std::make_shared<Service<Interface, Implementation>>(ServiceType::Transient, factory);
        m_services.push_back(service);
        return *this;
    }

    template<std::meta::info Namespace>
    void ServiceBuilder::addController()
    {
        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto controllers = ControllerDiscovery<Namespace>::controllers_list;

        template for (constexpr auto _controller : controllers) {
            constexpr auto annotate_func = define_static_array(std::meta::members_of(_controller, ctx) | std::views::filter([] (std::meta::info _info) {
                return std::meta::is_function(_info)
                    && meta::http::has_http_unique_annotation(_info)
                    && !is_operator_function(_info)
                    && has_identifier(_info);
            }));

            using ControllerType = [:_controller:];
            using RegisteredRouteType = RegisteredRoute<ControllerType>;
            using RegisteredControllerType = RegisteredRouteType::RegisteredControllerType;
            using SharedRegisteredControllerType = RegisteredRouteType::SharedRegisteredControllerType;
            using ControllerCtorInfoInternal = ControllerCtorInfo<ControllerType>;


            SharedRegisteredControllerType controller = std::make_shared<RegisteredControllerType>([] (std::shared_ptr<impl::IServiceProvider> &_service_provider) -> std::shared_ptr<ControllerType> {
                constexpr std::string_view controller_identifier = std::meta::identifier_of(_controller);
                ScopedContainer scoped_container{};

                auto tuple = make_parameters_tuple([controller_identifier, &_service_provider, &scoped_container] (auto _index) {
                    constexpr size_t i = decltype(_index)::value;
                    constexpr std::string_view interface_name = ControllerCtorInfoInternal::interface_names[i];
                    constexpr std::optional<std::meta::info> target_interface_opt = meta::extra::retreive_type<std::define_static_string(interface_name), ^^SERVICE_INTERFACE_NAMESPACE>();

                    static_assert(target_interface_opt.has_value(), "Unable to find the interface");

                    using TargetInterface = [:target_interface_opt.value():];

                    const std::shared_ptr<AService> &service_info = _service_provider->getServiceInfo(interface_name);
                    ServiceType service_type = service_info->getType();


                    if (service_type == ServiceType::Singleton) {
                        try {
                            return std::any_cast<std::shared_ptr<TargetInterface>>(_service_provider->getSingletonService(interface_name));
                        } catch (std::bad_any_cast _ex) {
                            std::ignore = _ex;

                            throw hsc::ControllerDIException("internal error: singleton cast failed", controller_identifier, interface_name);
                        }
                    }
                    if (service_type == ServiceType::Scoped && scoped_container.contains(interface_name)) {
                        try {
                            return std::any_cast<std::shared_ptr<TargetInterface>>(scoped_container.getService(interface_name));
                        } catch (std::bad_any_cast _ex) {
                            std::ignore = _ex;

                            throw hsc::ControllerDIException("internal error: registered scoped cast failed", controller_identifier, interface_name);
                        }
                    }

                    std::shared_ptr<AServiceWrapper<TargetInterface>> service_wrapper = std::static_pointer_cast<AServiceWrapper<TargetInterface>>(service_info);

                    if (!service_wrapper)
                        throw hsc::ControllerDIException("Unable to find the implementation of interface", controller_identifier, interface_name);

                    std::shared_ptr<TargetInterface> real_service = nullptr;

                    try {
                        real_service = service_wrapper->create(_service_provider, scoped_container);
                    } catch (const hsc::ServiceDIException &_ex) {
                        throw hsc::ControllerDIException("Service creation failed", controller_identifier, interface_name, std::make_unique<hsc::ServiceDIException>(_ex));
                    }

                    if (service_type == ServiceType::Scoped)
                        scoped_container.registerService(interface_name, real_service);
                    return real_service;
                }, std::make_index_sequence<ControllerCtorInfoInternal::params_size>{});

                return std::apply([&] (auto &&...tuple_args) {
                    return std::make_shared<ControllerType>(std::forward<decltype(tuple_args)>(tuple_args)...);
                }, tuple);
            });

            template for (constexpr std::meta::info _func : annotate_func) {
                constexpr std::string_view func_identifier = std::meta::identifier_of(_func);

                constexpr std::tuple<http::Method, std::string_view> route_info = meta::http::extract_route_type<_func>();
                constexpr http::Method route_type = std::get<http::Method>(route_info);
                constexpr std::string_view route = std::get<std::string_view>(route_info);

                // ICE if using a lambda on ControllerType mangling
                m_registered_routes.push_back(std::make_unique<RegisteredRouteType>(route, route_type, controller, GenerateCallback<ControllerType, _func>()));
            }
        }
    }

    template<class T, std::meta::info Func>
    consteval auto ServiceBuilder::GenerateCallback()
    {
        return [] (std::shared_ptr<T> _controller) {
            (*_controller).[:Func:]();
        };
    }

    template<size_t ...Is>
    constexpr auto ServiceBuilder::make_parameters_tuple(auto _fn, std::index_sequence<Is...>) {
        return std::make_tuple(_fn(std::integral_constant<size_t, Is>{})...);
    }

    template<IsServiceImplementation Implementation, size_t ArgsSize>
    consteval std::array<std::string_view, ServiceBuilder::ServiceCtorInfo<Implementation, ArgsSize>::params_service_size> ServiceBuilder::ServiceCtorInfo<Implementation, ArgsSize>::GetParametersTypeName()
    {
        constexpr auto service_params = define_static_array(params | std::views::take(params_service_size));
        std::array<std::string_view, params_service_size> out{};

        template for (size_t it = 0; constexpr std::meta::info _param : service_params) {
            constexpr std::meta::info type_info = type_of(_param);

            static_assert(std::meta::has_template_arguments(type_info), "Unable to determine the type parameter");
            static_assert(std::meta::template_of(type_info) == ^^std::shared_ptr, "For DI, parameter should be a shared_ptr");

            constexpr auto template_args = define_static_array(std::meta::template_arguments_of(type_info));
            constexpr std::string_view interface_name = std::meta::identifier_of(std::meta::dealias(template_args[0]));

            out[it++] = interface_name;
        }
        return out;
    }
}
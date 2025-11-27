#include "HSC/Exception/ControllerDIException.hpp"
#include "HSC/utils/ControllerDiscovery.hpp"
#include "HSC/utils/ControllerCtorInfo.hpp"
#include "HSC/ServiceBuilder.hpp"
#include "HSC/ScopedContainer.hpp"

#include "HTTP/Route.hpp"

#include "meta/extra.hpp"
#include "meta/http.hpp"
#include "meta/make_parameters_tuple.hpp"

namespace hsc
{
    template<IsInterface Interface, IsServiceImplementation Implementation, class ...Args>
        requires std::is_base_of_v<Interface, Implementation>
    ServiceBuilder &ServiceBuilder::addSingleton(Args &&..._args)
    {
        return addSingletonInternal<Interface, Implementation, ^^SERVICE_INTERFACE_NAMESPACE>(std::forward<Args>(_args)...);
    }

    template<IsInterface Interface, IsServiceImplementation Implementation, class ...Args>
        requires std::is_base_of_v<Interface, Implementation>
    ServiceBuilder &ServiceBuilder::addScoped(Args &&..._args)
    {
        return addScopedInternal<Interface, Implementation, ^^SERVICE_INTERFACE_NAMESPACE>(std::forward<Args>(_args)...);
    }

    template<IsInterface Interface, IsServiceImplementation Implementation, class ...Args>
        requires std::is_base_of_v<Interface, Implementation>
    ServiceBuilder &ServiceBuilder::addTransient(Args &&..._args)
    {
        return addTransientInternal<Interface, Implementation, ^^SERVICE_INTERFACE_NAMESPACE>(std::forward<Args>(_args)...);
    }

    template<std::meta::info Namespace>
    void ServiceBuilder::addController()
    {
        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto controllers = ControllerDiscovery<Namespace>::controllers_list;

        template for (constexpr auto _controller : controllers) {
            constexpr auto annotate_func = define_static_array(std::meta::members_of(_controller, ctx)
                | std::views::filter([] (std::meta::info _info) {
                    return std::meta::is_function(_info)
                        && meta::http::has_http_unique_annotation(_info)
                        && !is_operator_function(_info)
                        && has_identifier(_info);
                })
            );

            using ControllerType = [:_controller:];
            using RegisteredRouteType = RegisteredRoute<ControllerType>;
            using RegisteredControllerType = RegisteredRouteType::RegisteredControllerType;
            using SharedRegisteredControllerType = RegisteredRouteType::SharedRegisteredControllerType;

            SharedRegisteredControllerType controller = std::make_shared<RegisteredControllerType>([] (
                std::shared_ptr<impl::IServiceProvider> &_service_provider
            ) -> std::shared_ptr<ControllerType> {
                ScopedContainer scoped_container{};

                auto tuple = TupleCreator::CreateControllerTuple<ControllerType>(_service_provider, scoped_container);

                return std::apply([&] (auto &&..._tuple_args) {
                    return std::make_shared<ControllerType>(std::forward<decltype(_tuple_args)>(_tuple_args)...);
                }, tuple);
            });

            template for (constexpr std::meta::info _func : annotate_func) {
                constexpr std::string_view func_identifier = std::meta::identifier_of(_func);

                constexpr std::tuple<http::Method, std::string_view> route_info = meta::http::extract_route_type<_func>();
                constexpr http::Method route_type = std::get<http::Method>(route_info);
                constexpr std::string_view route = std::get<std::string_view>(route_info);

                // ICE if using a lambda on ControllerType mangling
                m_registered_routes.push_back(
                    std::make_unique<RegisteredRouteType>(
                        route, route_type, controller, GenerateCallback<ControllerType, _func>()
                    )
                );
            }
        }
    }

    template<IsInterface Interface, IsServiceImplementation Implementation, std::meta::info Namespace, class ...Args>
        requires IsMetaNamespace<Namespace>
    ServiceBuilder &ServiceBuilder::addSingletonInternal(Args &&..._args)
    {
        using StoredTuple = std::tuple<std::decay_t<Args>...>;

        ServiceCreatorCallback<Interface> factory = [__args = std::make_shared<StoredTuple>(std::forward<Args>(_args)...)] (
            std::shared_ptr<impl::IServiceProvider> &_service_provider,
            ScopedContainer &_scoped_container
        ) mutable -> std::shared_ptr<Interface> {
            std::ignore = _scoped_container;

            return std::apply(
                [&] (auto &&..._tuple_args) {
                    return std::apply(
                        [&] (auto &...___args) {
                            return std::make_shared<Implementation>(
                                std::forward<decltype(_tuple_args)>(_tuple_args)...,
                                std::move(___args)...
                            );
                        },
                        *__args
                    );
                },
                TupleCreator::CreateSingletonTuple<Interface, Implementation, sizeof...(Args), Namespace>(_service_provider)
            );
        };

        std::shared_ptr<Service<Interface, Implementation>> service
            = std::make_shared<Service<Interface, Implementation>>(ServiceType::Singleton, factory);
        m_services.push_back(service);
        return *this;
    }

    template<IsInterface Interface, IsServiceImplementation Implementation, std::meta::info Namespace, class ...Args>
        requires IsMetaNamespace<Namespace>
    ServiceBuilder &ServiceBuilder::addScopedInternal(Args &&..._args)
    {
        ServiceCreatorCallback<Interface> factory = [...__args = std::forward<Args>(_args)] (
            std::shared_ptr<impl::IServiceProvider> &_service_provider,
            ScopedContainer &_scoped_container
        ) mutable -> std::shared_ptr<Interface> {
            return std::apply(
                [&] (auto &&..._tuple_args) {
                    return std::make_shared<Implementation>(
                        std::forward<decltype(_tuple_args)>(_tuple_args)...,
                        std::forward<Args>(__args)...
                    );
                },
                TupleCreator::CreateScopedTuple<Interface, Implementation, sizeof...(Args), Namespace>(
                    _service_provider,
                    _scoped_container
                )
            );
        };

        std::shared_ptr<Service<Interface, Implementation>> service
            = std::make_shared<Service<Interface, Implementation>>(ServiceType::Scoped, factory);
        m_services.push_back(service);
        return *this;
    }

    template<IsInterface Interface, IsServiceImplementation Implementation, std::meta::info Namespace, class ...Args>
        requires IsMetaNamespace<Namespace>
    ServiceBuilder &ServiceBuilder::addTransientInternal(Args &&..._args)
    {
        ServiceCreatorCallback<Interface> factory = [...__args = std::forward<Args>(_args)] (
            std::shared_ptr<impl::IServiceProvider> &_service_provider,
            ScopedContainer &_scoped_container
        ) mutable -> std::shared_ptr<Interface> {
            return std::apply(
                [&] (auto &&..._tuple_args) {
                    return std::make_shared<Implementation>(
                        std::forward<decltype(_tuple_args)>(_tuple_args)...,
                        std::forward<Args>(__args)...
                    );
                },
                TupleCreator::CreateScopedTuple<Interface, Implementation, sizeof...(Args), Namespace>(
                    _service_provider,
                    _scoped_container
                )
            );
        };

        std::shared_ptr<Service<Interface, Implementation>> service
            = std::make_shared<Service<Interface, Implementation>>(ServiceType::Transient, factory);
        m_services.push_back(service);
        return *this;
    }

    template<IsInterface Interface, IsServiceImplementation Implementation, size_t ArgsSize, std::meta::info Namespace>
        requires IsMetaNamespace<Namespace>
    auto ServiceBuilder::TupleCreator::CreateSingletonTuple(std::shared_ptr<impl::IServiceProvider> &_service_provider)
    {
        using ServiceCtorInfoInternal = ServiceCtorInfo<Implementation, ArgsSize>;

        constexpr std::string_view interface_identifier = std::meta::identifier_of(^^Interface);
        constexpr std::string_view implementation_identifier = std::meta::identifier_of(^^Implementation);

        return meta::make_parameters_tuple([interface_identifier, implementation_identifier, &_service_provider] (auto _index) {
            constexpr size_t i = decltype(_index)::value;
            constexpr std::string_view interface_name = ServiceCtorInfoInternal::interface_names[i];

            static_assert(interface_name != interface_identifier, "Can't instantiate the service recursively");

            constexpr std::optional<std::meta::info> target_interface_opt
                = meta::extra::retreive_type<std::define_static_string(interface_name), Namespace>();

            static_assert(target_interface_opt.has_value(), "Unable to find the service interface");

            using TargetInterface = [:target_interface_opt.value():];

            ServiceType service_type = _service_provider->getServiceType(interface_name);

            if (service_type == ServiceType::Transient)
                throw hsc::ServiceDIException("You can't inject a transient in a singleton service",
                    implementation_identifier, interface_identifier, interface_name);
            if (service_type == ServiceType::Scoped)
                throw hsc::ServiceDIException("You can't inject a scoped in a singleton service",
                    implementation_identifier, interface_identifier, interface_name);

            try {
                return std::any_cast<std::shared_ptr<TargetInterface>>(_service_provider->getSingletonService(interface_name));
            } catch (std::bad_any_cast _ex) {
                std::ignore = _ex;

                throw hsc::ServiceDIException("internal error: singleton cast failed, from singleton service",
                    implementation_identifier, interface_identifier, interface_name);
            }
        }, std::make_index_sequence<ServiceCtorInfoInternal::params_size - ArgsSize>{});
    }

    template<IsInterface Interface, IsServiceImplementation Implementation, size_t ArgsSize, std::meta::info Namespace>
        requires IsMetaNamespace<Namespace>
    auto ServiceBuilder::TupleCreator::CreateScopedTuple(
        std::shared_ptr<impl::IServiceProvider> &_service_provider,
        ScopedContainer &_scoped_container
    )
    {
        using ServiceCtorInfoInternal = ServiceCtorInfo<Implementation, ArgsSize>;

        constexpr std::string_view interface_identifier = std::meta::identifier_of(^^Interface);
        constexpr std::string_view implementation_identifier = std::meta::identifier_of(^^Implementation);

        return meta::make_parameters_tuple([
            interface_identifier, implementation_identifier,
            &_service_provider, &_scoped_container
        ] (auto _index) {
            constexpr size_t i = decltype(_index)::value;
            constexpr std::string_view interface_name = ServiceCtorInfoInternal::interface_names[i];

            static_assert(interface_name != interface_identifier, "Can't instantiate the service recursively");

            constexpr std::optional<std::meta::info> target_interface_opt
                = meta::extra::retreive_type<std::define_static_string(interface_name), Namespace>();

            static_assert(target_interface_opt.has_value(), "Unable to find the service interface");

            using TargetInterface = [:target_interface_opt.value():];

            const std::shared_ptr<AService> &service_info = _service_provider->getServiceInfo(interface_name);
            ServiceType service_type = service_info->getType();

            if (service_type == ServiceType::Transient)
                throw hsc::ServiceDIException("You can't inject a transient in a scoped service",
                    implementation_identifier, interface_identifier, interface_name);
            if (service_type == ServiceType::Singleton) {
                try {
                    return std::any_cast<std::shared_ptr<TargetInterface>>(
                        _service_provider->getSingletonService(interface_name)
                    );
                } catch (std::bad_any_cast _ex) {
                    std::ignore = _ex;

                    throw hsc::ServiceDIException("internal error: singleton cast failed, from scoped service",
                        implementation_identifier, interface_identifier, interface_name);
                }
            }
            if (service_type == ServiceType::Scoped && _scoped_container.contains(interface_name)) {
                try {
                    return std::any_cast<std::shared_ptr<TargetInterface>>(_scoped_container.getService(interface_name));
                } catch (std::bad_any_cast _ex) {
                    std::ignore = _ex;

                    throw hsc::ServiceDIException("internal error: registered scoped cast failed, from scoped service",
                        implementation_identifier, interface_identifier, interface_name);
                }
            }

            std::shared_ptr<AServiceWrapper<TargetInterface>> service_wrapper
                = std::static_pointer_cast<AServiceWrapper<TargetInterface>>(service_info);

            if (!service_wrapper)
                throw hsc::ServiceDIException("Unable to find the implementation of the interface",
                    implementation_identifier, interface_identifier, interface_name);

            std::shared_ptr<TargetInterface> real_service = nullptr;

            try {
                real_service = service_wrapper->create(_service_provider, _scoped_container);
            } catch (hsc::ServiceDIException &_ex) {
                throw hsc::ServiceDIException("service creation failed", implementation_identifier,
                    interface_identifier, interface_name, std::make_unique<hsc::ServiceDIException>(std::move(_ex)));
            }
            _scoped_container.registerService(interface_name, real_service);
            return real_service;
        }, std::make_index_sequence<ServiceCtorInfoInternal::params_size - ArgsSize>{});
    }

    template<IsInterface Interface, IsServiceImplementation Implementation, size_t ArgsSize, std::meta::info Namespace>
        requires IsMetaNamespace<Namespace>
    auto ServiceBuilder::TupleCreator::CreateTransientTuple(
        std::shared_ptr<impl::IServiceProvider> &_service_provider,
        ScopedContainer &_scoped_container
    )
    {
        using ServiceCtorInfoInternal = ServiceCtorInfo<Implementation, ArgsSize>;

        constexpr std::string_view interface_identifier = std::meta::identifier_of(^^Interface);
        constexpr std::string_view implementation_identifier = std::meta::identifier_of(^^Implementation);

        return meta::make_parameters_tuple([
            interface_identifier, implementation_identifier,
            &_service_provider, &_scoped_container
        ] (auto _index) {
            constexpr size_t i = decltype(_index)::value;
            constexpr std::string_view interface_name = ServiceCtorInfoInternal::interface_names[i];

            static_assert(interface_name != interface_identifier, "Can't instantiate the service recursively");

            constexpr std::optional<std::meta::info> target_interface_opt
                = meta::extra::retreive_type<std::define_static_string(interface_name), Namespace>();

            static_assert(target_interface_opt.has_value(), "Unable to find the service interface");

            using TargetInterface = [:target_interface_opt.value():];

            const std::shared_ptr<AService> &service_info = _service_provider->getServiceInfo(interface_name);
            ServiceType service_type = service_info->getType();

            if (service_type == ServiceType::Singleton) {
                try {
                    return std::any_cast<std::shared_ptr<TargetInterface>>(
                        _service_provider->getSingletonService(interface_name)
                    );
                } catch (std::bad_any_cast _ex) {
                    std::ignore = _ex;

                    throw hsc::ServiceDIException("internal error: singleton cast failed, from transient service",
                        implementation_identifier, interface_identifier, interface_name);
                }
            }
            if (service_type == ServiceType::Scoped && _scoped_container.contains(interface_name)) {
                try {
                    return std::any_cast<std::shared_ptr<TargetInterface>>(_scoped_container.getService(interface_name));
                } catch (std::bad_any_cast _ex) {
                    std::ignore = _ex;

                    throw hsc::ServiceDIException("internal error: registered scoped cast failed, from transient service",
                        implementation_identifier, interface_identifier, interface_name);
                }
            }

            std::shared_ptr<AServiceWrapper<TargetInterface>> service_wrapper
                = std::static_pointer_cast<AServiceWrapper<TargetInterface>>(service_info);

            if (!service_wrapper)
                throw hsc::ServiceDIException("Unable to find the implementation of the interface",
                    implementation_identifier, interface_identifier, interface_name);

            std::shared_ptr<TargetInterface> real_service = nullptr;

            try {
                real_service = service_wrapper->create(_service_provider, _scoped_container);
            } catch (hsc::ServiceDIException &_ex) {
                throw hsc::ServiceDIException("service creation failed", implementation_identifier,
                    interface_identifier, interface_name, std::make_unique<hsc::ServiceDIException>(std::move(_ex)));
            }
            if (service_type == ServiceType::Scoped)
                _scoped_container.registerService(interface_name, real_service);
            return real_service;
        }, std::make_index_sequence<ServiceCtorInfoInternal::params_size - ArgsSize>{});
    }

    template<IsController Controller>
    auto ServiceBuilder::TupleCreator::CreateControllerTuple(
        std::shared_ptr<impl::IServiceProvider> &_service_provider,
        ScopedContainer &_scoped_container
    )
    {
        using ControllerCtorInfoInternal = ControllerCtorInfo<Controller>;

        constexpr std::string_view controller_identifier = std::meta::identifier_of(^^Controller);

        return meta::make_parameters_tuple([controller_identifier, &_service_provider, &_scoped_container] (auto _index) {
            constexpr size_t i = decltype(_index)::value;
            constexpr std::string_view interface_name = ControllerCtorInfoInternal::interface_names[i];
            constexpr std::optional<std::meta::info> target_interface_opt
                = meta::extra::retreive_type<std::define_static_string(interface_name), ^^SERVICE_INTERFACE_NAMESPACE>();

            static_assert(target_interface_opt.has_value(), "Unable to find the interface");

            using TargetInterface = [:target_interface_opt.value():];

            const std::shared_ptr<AService> &service_info = _service_provider->getServiceInfo(interface_name);
            ServiceType service_type = service_info->getType();

            if (service_type == ServiceType::Singleton) {
                try {
                    return std::any_cast<std::shared_ptr<TargetInterface>>(
                        _service_provider->getSingletonService(interface_name)
                    );
                } catch (std::bad_any_cast _ex) {
                    std::ignore = _ex;

                    throw hsc::ControllerDIException("internal error: singleton cast failed",
                        controller_identifier, interface_name);
                }
            }
            if (service_type == ServiceType::Scoped && _scoped_container.contains(interface_name)) {
                try {
                    return std::any_cast<std::shared_ptr<TargetInterface>>(_scoped_container.getService(interface_name));
                } catch (std::bad_any_cast _ex) {
                    std::ignore = _ex;

                    throw hsc::ControllerDIException("internal error: registered scoped cast failed",
                        controller_identifier, interface_name);
                }
            }

            std::shared_ptr<AServiceWrapper<TargetInterface>> service_wrapper
                = std::static_pointer_cast<AServiceWrapper<TargetInterface>>(service_info);

            if (!service_wrapper)
                throw hsc::ControllerDIException("Unable to find the implementation of interface",
                    controller_identifier, interface_name);

            std::shared_ptr<TargetInterface> real_service = nullptr;

            try {
                real_service = service_wrapper->create(_service_provider, _scoped_container);
            } catch (const hsc::ServiceDIException &_ex) {
                throw hsc::ControllerDIException("Service creation failed", controller_identifier,
                    interface_name, std::make_unique<hsc::ServiceDIException>(_ex));
            }

            if (service_type == ServiceType::Scoped)
                _scoped_container.registerService(interface_name, real_service);
            return real_service;
        }, std::make_index_sequence<ControllerCtorInfoInternal::params_size>{});
    }

    template<class T, std::meta::info Func>
    consteval auto ServiceBuilder::GenerateCallback()
    {
        return [] (std::shared_ptr<T> _controller) {
            (*_controller).[:Func:]();
        };
    }
}
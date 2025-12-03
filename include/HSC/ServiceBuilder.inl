#include "HSC/Exception/ControllerDIException.hpp"
#include "HSC/Exception/ServiceDIException.hpp"
#include "HSC/utils/ControllerDiscovery.hpp"
#include "HSC/utils/ControllerCtorInfo.hpp"
#include "HSC/utils/RoutingParameter.hpp"
#include "HSC/ServiceBuilder.hpp"

#include "HTTP/Route.hpp"

#include "meta/extra.hpp"
#include "meta/http.hpp"
#include "meta/make_parameters_tuple.hpp"

namespace hsc
{
    template<IsServiceInterface Interface, IsServiceImplementation Implementation, class ...Args>
        requires std::is_base_of_v<Interface, Implementation>
    ServiceBuilder &ServiceBuilder::addSingleton(Args &&..._args)
    {
        using StoredTuple = std::tuple<std::decay_t<Args>...>;

        ServiceCreatorCallback<Interface> factory = [__args = std::make_shared<StoredTuple>(std::forward<Args>(_args)...)] (
            std::shared_ptr<impl::AServiceProvider> &_service_provider
        ) mutable -> std::shared_ptr<Interface> {
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
                TupleCreator::CreateSingletonTuple<Interface, Implementation, sizeof...(Args)>(_service_provider)
            );
        };

        std::shared_ptr<Service<Interface, Implementation>> service
            = std::make_shared<Service<Interface, Implementation>>(ServiceType::Singleton, factory);
        m_services.push_back(service);
        return *this;
    }

    template<IsServiceInterface Interface, IsServiceImplementation Implementation, class ...Args>
        requires std::is_base_of_v<Interface, Implementation>
    ServiceBuilder &ServiceBuilder::addScoped(Args &&..._args)
    {
        ServiceCreatorCallback<Interface> factory = [...__args = std::forward<Args>(_args)] (
            std::shared_ptr<impl::AServiceProvider> &_service_provider
        ) mutable -> std::shared_ptr<Interface> {
            return std::apply(
                [&] (auto &&..._tuple_args) {
                    return std::make_shared<Implementation>(
                        std::forward<decltype(_tuple_args)>(_tuple_args)...,
                        std::forward<Args>(__args)...
                    );
                },
                TupleCreator::CreateScopedTuple<Interface, Implementation, sizeof...(Args)>(_service_provider)
            );
        };

        std::shared_ptr<Service<Interface, Implementation>> service
            = std::make_shared<Service<Interface, Implementation>>(ServiceType::Scoped, factory);
        m_services.push_back(service);
        return *this;
    }

    template<IsServiceInterface Interface, IsServiceImplementation Implementation, class ...Args>
        requires std::is_base_of_v<Interface, Implementation>
    ServiceBuilder &ServiceBuilder::addTransient(Args &&..._args)
    {
        ServiceCreatorCallback<Interface> factory = [...__args = std::forward<Args>(_args)] (
            std::shared_ptr<impl::AServiceProvider> &_service_provider
        ) mutable -> std::shared_ptr<Interface> {
            return std::apply(
                [&] (auto &&..._tuple_args) {
                    return std::make_shared<Implementation>(
                        std::forward<decltype(_tuple_args)>(_tuple_args)...,
                        std::forward<Args>(__args)...
                    );
                },
                TupleCreator::CreateScopedTuple<Interface, Implementation, sizeof...(Args)>(_service_provider)
            );
        };

        std::shared_ptr<Service<Interface, Implementation>> service
            = std::make_shared<Service<Interface, Implementation>>(ServiceType::Transient, factory);
        m_services.push_back(service);
        return *this;
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
            using RegisteredControllerType = RegisteredController<ControllerType>;
            using SharedRegisteredControllerType = std::shared_ptr<RegisteredControllerType>;

            SharedRegisteredControllerType controller = std::make_shared<RegisteredControllerType>([] (
                std::shared_ptr<impl::AServiceProvider> &_service_provider
            ) -> std::shared_ptr<ControllerType> {
                auto tuple = TupleCreator::CreateControllerTuple<ControllerType>(_service_provider);

                return std::apply([&] (auto &&..._tuple_args) {
                    return std::make_shared<ControllerType>(std::forward<decltype(_tuple_args)>(_tuple_args)...);
                }, tuple);
            });

            template for (constexpr std::meta::info _func : annotate_func) {
                constexpr std::string_view func_identifier = std::meta::identifier_of(_func);

                constexpr std::tuple<http::Method, std::string_view> route_info = meta::http::extract_route_type<_func>();
                constexpr http::Method route_type = std::get<http::Method>(route_info);
                constexpr std::string_view route = std::get<std::string_view>(route_info);
                constexpr auto params = std::define_static_array(std::meta::parameters_of(_func));

                using RegisteredRouteType = RegisteredRoute<ControllerType>;
                using RouteParametersInternal = RoutingParameter<route.data()>;

                if constexpr (RouteParametersInternal::params_size != params.size())
                    static_assert(false, "Wrong number of parameter");

                template for (constexpr std::meta::info _param : params) {
                    if constexpr (!std::meta::has_identifier(_param))
                        static_assert(false, "Parameter should be named");

                    constexpr std::string_view param_identifier = std::meta::identifier_of(_param);

                    if constexpr (!RouteParametersInternal::MatchParameter(param_identifier))
                        static_assert(false, "Parameter not found");
                }

                // ICE if using a lambda on ControllerType mangling
                m_registered_routes.push_back(
                    std::make_unique<RegisteredRouteType>(
                        route, route_type, controller, GenerateCallback<ControllerType, _func>()
                    )
                );
            }
        }
    }

    template<IsServiceInterface Interface, IsServiceImplementation Implementation, size_t ArgsSize>
    auto ServiceBuilder::TupleCreator::CreateSingletonTuple(std::shared_ptr<impl::AServiceProvider> &_service_provider)
    {
        using ServiceCtorInfoInternal = ServiceCtorInfo<Implementation, ArgsSize>;

        constexpr std::string_view interface_identifier = std::meta::identifier_of(^^Interface);
        constexpr std::string_view implementation_identifier = std::meta::identifier_of(^^Implementation);

        return meta::make_parameters_tuple([interface_identifier, implementation_identifier, &_service_provider] (auto _index) {
            constexpr size_t i = decltype(_index)::value;
            constexpr std::meta::info interface_info = ServiceCtorInfoInternal::interface_info[i];
            constexpr std::string_view interface_name = std::meta::identifier_of(std::meta::dealias(interface_info));

            using TargetInterface = [:interface_info:];

            static_assert(!std::is_same_v<TargetInterface, Interface>, "Can't instantiate the service recursively");

            ServiceType service_type = _service_provider->getServiceType(interface_name);

            if (service_type == ServiceType::Transient)
                throw ServiceDIException("You can't inject a transient in a singleton service",
                    interface_identifier, implementation_identifier, interface_name);
            if (service_type == ServiceType::Scoped)
                throw ServiceDIException("You can't inject a scoped in a singleton service",
                    interface_identifier, implementation_identifier, interface_name);

            try {
                return _service_provider->getService<TargetInterface>();
            } catch (hsc::ServiceException &_ex) {
                throw ServiceDIException("Error during service instantiation, in singleton instantiation",
                    interface_identifier, implementation_identifier, interface_name);
            }
        }, std::make_index_sequence<ServiceCtorInfoInternal::params_size - ArgsSize>{});
    }

    template<IsServiceInterface Interface, IsServiceImplementation Implementation, size_t ArgsSize>
    auto ServiceBuilder::TupleCreator::CreateScopedTuple(std::shared_ptr<impl::AServiceProvider> &_service_provider)
    {
        using ServiceCtorInfoInternal = ServiceCtorInfo<Implementation, ArgsSize>;

        constexpr std::string_view interface_identifier = std::meta::identifier_of(^^Interface);
        constexpr std::string_view implementation_identifier = std::meta::identifier_of(^^Implementation);

        return meta::make_parameters_tuple([interface_identifier, implementation_identifier, &_service_provider] (auto _index) {
            constexpr size_t i = decltype(_index)::value;
            constexpr std::meta::info interface_info = ServiceCtorInfoInternal::interface_info[i];
            constexpr std::string_view interface_name = std::meta::identifier_of(std::meta::dealias(interface_info));

            using TargetInterface = [:interface_info:];

            static_assert(!std::is_same_v<TargetInterface, Interface>, "Can't instantiate the service recursively");

            const std::shared_ptr<AService> &service_info = _service_provider->getServiceInfo(interface_name);
            ServiceType service_type = service_info->getType();

            if (service_type == ServiceType::Transient)
                throw ServiceDIException("You can't inject a transient in a scoped service",
                    interface_identifier, implementation_identifier, interface_name);

            try {
                return _service_provider->getService<TargetInterface>();
            } catch (ServiceException &_ex) {
                throw ServiceDIException("Error during service instantiation, in singleton instantiation",
                    interface_identifier, implementation_identifier, interface_name);
            }
        }, std::make_index_sequence<ServiceCtorInfoInternal::params_size - ArgsSize>{});
    }

    template<IsServiceInterface Interface, IsServiceImplementation Implementation, size_t ArgsSize>
    auto ServiceBuilder::TupleCreator::CreateTransientTuple(std::shared_ptr<impl::AServiceProvider> &_service_provider)
    {
        using ServiceCtorInfoInternal = ServiceCtorInfo<Implementation, ArgsSize>;

        constexpr std::string_view interface_identifier = std::meta::identifier_of(^^Interface);
        constexpr std::string_view implementation_identifier = std::meta::identifier_of(^^Implementation);

        return meta::make_parameters_tuple([interface_identifier, implementation_identifier, &_service_provider] (auto _index) {
            constexpr size_t i = decltype(_index)::value;
            constexpr std::meta::info interface_info = ServiceCtorInfoInternal::interface_info[i];
            constexpr std::string_view interface_name = std::meta::identifier_of(std::meta::dealias(interface_info));

            using TargetInterface = [:interface_info:];

            static_assert(!std::is_same_v<TargetInterface, Interface>, "Can't instantiate the service recursively");

            try {
                return _service_provider->getService<TargetInterface>();
            } catch (ServiceException &_ex) {
                throw ServiceDIException("Error during service instantiation, in transient instantiation",
                    interface_identifier, implementation_identifier, interface_name);
            }
        }, std::make_index_sequence<ServiceCtorInfoInternal::params_size - ArgsSize>{});
    }

    template<IsController Controller>
    auto ServiceBuilder::TupleCreator::CreateControllerTuple(std::shared_ptr<impl::AServiceProvider> &_service_provider)
    {
        using ControllerCtorInfoInternal = ControllerCtorInfo<Controller>;

        constexpr std::string_view controller_identifier = std::meta::identifier_of(^^Controller);

        return meta::make_parameters_tuple([controller_identifier, &_service_provider] (auto _index) {
            constexpr size_t i = decltype(_index)::value;
            constexpr std::meta::info interface_info = ControllerCtorInfoInternal::interface_info[i];
            constexpr std::string_view interface_name = std::meta::identifier_of(std::meta::dealias(interface_info));

            using TargetInterface = [:interface_info:];

            try {
                return _service_provider->getService<TargetInterface>();
            } catch (ServiceException &_ex) {
                throw ControllerDIException("Service creation failed", controller_identifier,
                    interface_name, std::make_unique<ServiceException>(_ex));
            }
        }, std::make_index_sequence<ControllerCtorInfoInternal::params_size>{});
    }

    template<class T, std::meta::info Func>
    consteval auto ServiceBuilder::GenerateCallback()
    {
        return [] (std::shared_ptr<T> _controller) -> http::Response {
            // return (*_controller).[:Func:]();
            return http::Response{};
        };
    }
}
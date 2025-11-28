#include "HSC/utils/MiddlewareCtorInfo.hpp"
#include "HSC/ServiceCollection.hpp"


#include "meta/extra.hpp"
#include "meta/make_parameters_tuple.hpp"

namespace hsc
{
    template<IsMiddleware MW, class ...Args>
    ServiceCollection &ServiceCollection::addMiddleware(Args &&..._args)
    {
        using StoredTuple = std::tuple<std::decay_t<Args>...>;

        typename Middleware<MW>::Ctor factory = [__args = std::make_shared<StoredTuple>(std::forward<Args>(_args)...)] (
            MiddlewareCallback _cb,
            std::shared_ptr<impl::IServiceProvider> &_service_provider
        ) -> std::unique_ptr<MW> {
            return std::apply(
                [&] (auto &&..._tuple_args) {
                    return std::apply(
                        [&] (auto &...___args) {
                            return std::make_unique<MW>(
                                std::forward<decltype(_tuple_args)>(_tuple_args)...,
                                std::move(___args)...
                            );
                        },
                        *__args
                    );
                },
                TupleCreator::CreateMiddlewareTuple<MW, sizeof...(Args), ^^::hsc::impl>(_cb, _service_provider)
            );
        };

        std::function<void(std::unique_ptr<MW> &, http::Context &)> invoke_func = [] (std::unique_ptr<MW> &_middleware, http::Context &_context) {
            constexpr std::meta::info invoke = meta::extra::get_invoke_function<^^MW>();

            (*_middleware).[:invoke:](_context);
        };

        m_registered_middlewares.push_back(std::make_unique<Middleware<MW>>(factory, invoke_func));
        return *this;
    }

    template<IsMiddleware MW, size_t ArgsSize, std::meta::info Namespace>
        requires IsMetaNamespace<Namespace>
    auto ServiceCollection::TupleCreator::CreateMiddlewareTuple(
        MiddlewareCallback _cb,
        std::shared_ptr<impl::IServiceProvider> &_service_provider
    )
    {
        using MiddlewareCtorInfoInternal = MiddlewareCtorInfo<MW, ArgsSize>;

        return meta::make_parameters_tuple([_cb, &_service_provider] (auto _index) {
            constexpr size_t i = decltype(_index)::value;

            if constexpr (i == 0) {
                return _cb;
            } else {
                constexpr std::string_view interface_name = MiddlewareCtorInfoInternal::interface_names[i - 1];

                constexpr std::optional<std::meta::info> target_interface_opt
                    = meta::extra::retreive_type<std::define_static_string(interface_name), Namespace>();

                static_assert(target_interface_opt.has_value(), "Unable to find the service interface");

                using TargetInterface = [:target_interface_opt.value():];

                ServiceType service_type = _service_provider->getServiceType(interface_name);

                if (service_type == ServiceType::Transient)
                    throw "You can't inject a transient service in a middleware constructor";
                if (service_type == ServiceType::Scoped)
                    throw "You can't inject a scoped service in a middleware constructor";
                try {
                    return std::any_cast<std::shared_ptr<TargetInterface>>(
                        _service_provider->getSingletonService(interface_name)
                    );
                } catch (std::bad_any_cast _ex) {
                    std::ignore = _ex;

                    throw "internal error: singleton cast failed, from middleware";
                }
            }
        }, std::make_index_sequence<MiddlewareCtorInfoInternal::params_size - ArgsSize>{});
    }

}
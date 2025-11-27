#include "HSC/ServiceCollection.hpp"

namespace hsc
{
    template<IsMiddleware MW>
    ServiceCollection &addMiddleware(Args &&..._args)
    {
        Middleware<MW>::Ctor factory = [...__args = std::forward<Args>(_args)] (
            MiddlewareCallback _cb,
            std::shared_ptr<impl::IServiceProvider> &_service_provider
        ) -> std::shared_ptr<MW> {
            auto tuple = make_parameters_tuple([] (auto index) {
                constexpr size_t i = decltype(_index)::value;

                if constexpr (i == 0) {
                    return _cb;
                } else {
                }
            }, std::make_index_sequence<ServiceCtorInfoInternal::params_size - sizeof...(Args)>{})

            std::apply([&] (auto &&_tuple_args) {
                return std::make_shared<MW>(
                    std::forward<decltype(_tuple_args)>(_tuple_args)...,
                    std::forward<Args>(__args)...
                );
            }, tuple);
        };

        return std::make_shared<Middleware<MW>>(factory);
    }
}
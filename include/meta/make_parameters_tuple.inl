#include "meta/make_parameters_tuple.hpp"

namespace meta
{
    template<size_t ...Is>
    constexpr auto make_parameters_tuple(auto _fn, std::index_sequence<Is...>) {
        return std::make_tuple(_fn(std::integral_constant<size_t, Is>{})...);
    }
}
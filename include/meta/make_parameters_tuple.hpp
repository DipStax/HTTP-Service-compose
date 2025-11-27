#pragma once

namespace meta
{
    template<size_t ...Is>
    constexpr auto make_parameters_tuple(auto _fn, std::index_sequence<Is...>);
}

#include "meta/make_parameters_tuple.inl"
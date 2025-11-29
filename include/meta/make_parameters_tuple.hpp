#pragma once

namespace meta
{
    /// @brief Create a tuple using the callback function to generate the different value of the tuple
    /// @tparam ...Is Index sequence to indicate the current index
    /// @param _fn Callback function generating a value
    /// @return The tuple of value from the function
    template<size_t ...Is>
    constexpr auto make_parameters_tuple(auto _fn, std::index_sequence<Is...>);
}

#include "meta/make_parameters_tuple.inl"
#pragma once

#include "meta/metaconcept.hpp"

namespace hsc
{
    template<std::meta::info Func>
        requires IsMetaFunction<Func>
    struct InvokeInfo
    {
        static constexpr auto params = define_static_array(std::meta::parameters_of(Func));
        static constexpr size_t params_size = params.size() - 1;

        static consteval std::array<std::string_view, params_size> GetParametersTypeName();

        static constexpr std::array<std::string_view, params_size> interface_names = GetParametersTypeName();
    };
}

#include "HSC/utils/InvokeInfo.inl"
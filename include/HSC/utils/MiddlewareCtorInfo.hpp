#pragma once

namespace hsc
{
    template<IsMiddleware MW, size_t ArgsSize>
    struct MiddlewareCtorInfo
    {
        static constexpr std::meta::info ctor = meta::extra::get_unique_ctor<^^MW>();
        static constexpr auto params = define_static_array(std::meta::parameters_of(ctor));
        static constexpr size_t params_size = params.size();
        static constexpr size_t params_service_size = params_size - ArgsSize - 1;

        static consteval std::array<std::string_view, params_service_size> GetParametersTypeName();

        static constexpr std::array<std::string_view, params_service_size> interface_names = GetParametersTypeName();
    };
}

#include "HSC/utils/MiddlewareCtorInfo.inl"
#pragma once

namespace hsc
{
    template<IsServiceImplementation Implementation, size_t ArgsSize>
    struct ServiceCtorInfo
    {
        static constexpr std::meta::info ctor = meta::extra::get_unique_ctor<^^Implementation>();
        static constexpr auto params = define_static_array(std::meta::parameters_of(ctor));
        static constexpr size_t params_size = params.size();
        static constexpr size_t params_service_size = params_size - ArgsSize;

        static consteval std::array<std::meta::info, params_service_size> GetParametersTypeName();

        static constexpr std::array<std::meta::info, params_service_size> interface_info = GetParametersTypeName();
    };
}

#include "HSC/utils/ServiceCtorInfo.inl"
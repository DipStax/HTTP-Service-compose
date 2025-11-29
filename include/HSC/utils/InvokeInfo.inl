#include "HSC/utils/InvokeInfo.hpp"

namespace hsc
{
    template<std::meta::info Func>
        requires IsMetaFunction<Func>
    consteval std::array<std::meta::info, InvokeInfo<Func>::params_size> InvokeInfo<Func>::GetParametersTypeName()
    {
        constexpr auto func_params = define_static_array(params | std::views::drop(1));
        std::array<std::meta::info, params_size> out{};

        template for (size_t it = 0; constexpr std::meta::info _param : func_params) {
            constexpr std::meta::info type_info = type_of(_param);

            static_assert(std::meta::has_template_arguments(type_info), "Unable to determine the type parameter");
            static_assert(std::meta::template_of(type_info) == ^^std::shared_ptr, "For DI, parameter should be a shared_ptr");

            constexpr auto template_args = define_static_array(std::meta::template_arguments_of(type_info));

            out[it++] = template_args[0];
        }
        return out;
    }
}
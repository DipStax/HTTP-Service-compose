#include "HSC/utils/MiddlewareCtorInfo.hpp"

namespace hsc
{
    template<ISMiddleware MW, size_t ArgsSize>
    consteval std::array<std::string_view, MiddlewareCtorInfo<MW>::params_service_size> MiddlewareCtorInfo<MW>::GetParametersTypeName()
    {
        constexpr auto service_params = define_static_array(params
                | std::views::drop(1)
                | std::views::take(params_service_size)
            );
        std::array<std::string_view, params_service_size> out{};

        template for (size_t it = 0; constexpr std::meta::info _param : service_params) {
            constexpr std::meta::info type_info = type_of(_param);

            static_assert(std::meta::has_template_arguments(type_info), "Unable to determine the type parameter");
            static_assert(std::meta::template_of(type_info) == ^^std::shared_ptr, "For DI, parameter should be a shared_ptr");

            constexpr auto template_args = define_static_array(std::meta::template_arguments_of(type_info));
            constexpr std::string_view interface_name = std::meta::identifier_of(std::meta::dealias(template_args[0]));

            out[it++] = interface_name;
        }
        return out;
    }
}
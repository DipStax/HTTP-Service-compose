#pragma once

#include <meta>

namespace extra::meta
{
    template<std::meta::info Type>
    consteval std::meta::info get_base_interface()
    {
        constexpr std::meta::access_context ctx = std::meta::access_context::current();
        constexpr auto bases = define_static_array(std::meta::bases_of(Type, ctx));

        template for (constexpr std::meta::info _base : bases) {
            constexpr std::meta::info type = type_of(_base);

            if constexpr (is_interface<type>())
                return type;
        }
        throw "Unable to find the base interface";
    }
}
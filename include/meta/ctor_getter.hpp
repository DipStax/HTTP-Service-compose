#pragma once

#include <experimental/meta>
#include <algorithm>

namespace extra::meta
{
    template<std::meta::info Type>
    consteval std::optional<std::meta::info> get_custom_ctor()
    {
        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto members = define_static_array(std::meta::members_of(Type, ctx));
        size_t count = 0;

        template for (constexpr std::meta::info _info : members)
            if (is_function(_info) && !is_special_member_function(_info) && is_constructor(_info))
                return _info;
        return std::nullopt;
    }

    template<std::meta::info Type>
    consteval std::optional<std::meta::info> get_default_ctor()
    {
        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto members = define_static_array(std::meta::members_of(Type, ctx));
        size_t count = 0;

        template for (constexpr std::meta::info _info : members)
            if (is_special_member_function(_info) && is_constructor(_info) && is_default_constructor(_info))
                return _info;
        return std::nullopt;
    }

    template<std::meta::info Type>
    consteval std::meta::info get_unique_ctor()
    {
        std::optional<std::meta::info> custom_ctor = get_custom_ctor<Type>();

        if (custom_ctor.has_value())
            return custom_ctor.value();
        return get_default_ctor<Type>().value();
    }
}
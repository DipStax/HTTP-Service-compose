#pragma once

#include <meta>

namespace extra::meta
{
    template<const char *Name, std::meta::info Namespace = ^^::>
    consteval std::optional<std::meta::info> retreive_type()
    {
        constexpr std::meta::access_context ctx = std::meta::access_context::current();
        constexpr auto members = define_static_array(std::meta::members_of(Namespace, ctx));

        template for (constexpr std::meta::info _member : members) {
            if constexpr (is_namespace(_member)) {
                constexpr std::optional<std::meta::info> found = retreive_type<Name, _member>();

                if constexpr (found.has_value())
                    return found.value();
            } else if constexpr (is_type(_member) && std::meta::has_identifier(_member)) {
                constexpr std::string_view identifier = std::meta::identifier_of(_member);

                if constexpr (identifier == Name)
                    return _member;
            }
        }
        return std::nullopt;
    }
}
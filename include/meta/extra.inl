#include <algorithm>

#include "meta/extra.hpp"

namespace meta::extra
{
    template<const char *Name, std::meta::info Namespace>
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

    template<std::meta::info T>
    consteval bool is_interface()
    {
        constexpr auto ctx = std::meta::access_context::unchecked();
        constexpr auto members = define_static_array(std::meta::members_of(T, ctx));

        for (std::meta::info member : members) {
            if (is_special_member_function(member))
                continue;
            if (is_nonstatic_data_member(member) || !std::meta::is_pure_virtual(member))
                return false;
        }
        return true;
    }

    template<std::meta::info T>
    consteval bool is_abstraction()
    {
        if (is_interface<T>())
            return false;

        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto members = define_static_array(std::meta::nonstatic_data_members_of(T, ctx));

        return std::ranges::count_if(members, std::meta::is_pure_virtual) > 0;
    }

    template<std::meta::info T>
    consteval bool has_default_constructor()
    {
        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto members = define_static_array(std::meta::members_of(T, ctx));
        size_t count = 0;

        template for (constexpr std::meta::info _info : members)
            if (is_special_member_function(_info) && is_constructor(_info) && is_default_constructor(_info))
                return true;
        return false;
    }

    template<std::meta::info T>
    consteval size_t count_constructor()
    {
        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto members = define_static_array(std::meta::members_of(T, ctx));
        size_t count = 0;

        return std::ranges::count_if(members, [] (std::meta::info _info) {
            return is_constructor(_info)
                && !is_copy_constructor(_info)
                && !is_move_constructor(_info);
        });
    }

    template<std::meta::info T>
    consteval std::optional<std::meta::info> get_first_custom_ctor()
    {
        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto members = define_static_array(std::meta::members_of(T, ctx));
        size_t count = 0;

        template for (constexpr std::meta::info _info : members)
            if (is_function(_info) && !is_special_member_function(_info) && is_constructor(_info))
                return _info;
        return std::nullopt;
    }

    template<std::meta::info T>
    consteval std::optional<std::meta::info> get_default_ctor()
    {
        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto members = define_static_array(std::meta::members_of(T, ctx));
        size_t count = 0;

        template for (constexpr std::meta::info _info : members)
            if (is_special_member_function(_info) && is_constructor(_info) && is_default_constructor(_info))
                return _info;
        return std::nullopt;
    }

    template<std::meta::info T>
    consteval std::meta::info get_unique_ctor()
    {
        std::optional<std::meta::info> custom_ctor = get_first_custom_ctor<T>();

        if (custom_ctor.has_value())
            return custom_ctor.value();
        return get_default_ctor<T>().value();
    }
}
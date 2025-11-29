#include <algorithm>

#include "meta/extra.hpp"

namespace meta::extra
{
    template<const char *Name, std::meta::info Namespace, std::meta::info ...OtherNamespaces>
        requires IsMetaNamespace<Namespace>
    consteval std::optional<std::meta::info> retreive_type()
    {
        constexpr std::meta::access_context ctx = std::meta::access_context::current();
        constexpr auto members = define_static_array(std::meta::members_of(Namespace, ctx));

        template for (constexpr std::meta::info _member : members) {
            if constexpr (is_namespace(_member)) {
                std::optional<std::meta::info> found = retreive_type<Name, _member>();

                if (found.has_value())
                    return found.value();
            } else if (is_type(_member) && std::meta::has_identifier(_member)) {
                std::string_view identifier = std::meta::identifier_of(_member);

                if (identifier == Name)
                    return _member;
            }
        }
        if constexpr (sizeof...(OtherNamespaces) != 0)
            return retreive_type<Name, OtherNamespaces...>();
        else
            return std::nullopt;
    }

    template<std::meta::info T>
        requires IsMetaType<T>
    consteval bool is_interface()
    {
        std::meta::access_context ctx = std::meta::access_context::unchecked();
        std::vector<std::meta::info> members = std::meta::members_of(T, ctx);

        for (std::meta::info member : members) {
            if (is_special_member_function(member))
                continue;
            if (is_nonstatic_data_member(member) || !std::meta::is_pure_virtual(member))
                return false;
        }
        return true;
    }

    template<std::meta::info T>
        requires IsMetaType<T>
    consteval bool is_abstraction()
    {
        if (is_interface<T>())
            return false;

        std::meta::access_context ctx = std::meta::access_context::unchecked();
        std::vector<std::meta::info> members = std::meta::nonstatic_data_members_of(T, ctx);

        return std::ranges::count_if(members, std::meta::is_pure_virtual) > 0;
    }

    template<std::meta::info T>
        requires IsMetaType<T>
    consteval bool has_default_constructor()
    {
        std::meta::access_context ctx = std::meta::access_context::current();
        std::vector<std::meta::info> members = std::meta::members_of(T, ctx);

        for (std::meta::info _info : members)
            if (is_special_member_function(_info) && is_constructor(_info) && is_default_constructor(_info))
                return true;
        return false;
    }

    template<std::meta::info T>
        requires IsMetaType<T>
    consteval size_t count_constructor()
    {
        constexpr auto ctx = std::meta::access_context::current();
        std::vector<std::meta::info> members = std::meta::members_of(T, ctx);

        return std::ranges::count_if(members, [] (std::meta::info _info) {
            return is_constructor(_info)
                && !is_copy_constructor(_info)
                && !is_move_constructor(_info);
        });
    }

    template<std::meta::info T>
        requires IsMetaType<T>
    consteval std::optional<std::meta::info> get_first_custom_ctor()
    {
        std::meta::access_context ctx = std::meta::access_context::current();
        std::vector<std::meta::info> members = std::meta::members_of(T, ctx);

        for (std::meta::info _member : members)
            if (is_function(_member) && !is_special_member_function(_member) && is_constructor(_member))
                return _member;
        return std::nullopt;
    }

    template<std::meta::info T>
        requires IsMetaType<T>
    consteval std::optional<std::meta::info> get_default_ctor()
    {
        std::meta::access_context ctx = std::meta::access_context::current();
        std::vector<std::meta::info> members = std::meta::members_of(T, ctx);

        for (std::meta::info _member : members)
            if (is_special_member_function(_member) && is_constructor(_member) && is_default_constructor(_member))
                return _member;
        return std::nullopt;
    }

    template<std::meta::info T>
        requires IsMetaType<T>
    consteval std::meta::info get_unique_ctor()
    {
        std::optional<std::meta::info> custom_ctor = get_first_custom_ctor<T>();

        if (custom_ctor.has_value())
            return custom_ctor.value();
        return get_default_ctor<T>().value();
    }

    template<class T, std::meta::info ParamType>
        requires IsMetaType<ParamType>
    consteval bool is_first_ctor_parameter()
    {
        std::meta::info ctor = get_unique_ctor<^^T>();
        std::vector<std::meta::info> params = std::meta::parameters_of(ctor);

        if (params.size() < 1)
            return false;
        return std::meta::is_same_type(std::meta::type_of(params[0]), ParamType);
    }

    template<std::meta::info T>
        requires IsMetaType<T>
    consteval std::meta::info get_invoke_function()
    {
        std::meta::access_context ctx = std::meta::access_context::current();
        std::vector<std::meta::info> members = std::meta::members_of(T, ctx);

        for (std::meta::info _member : members)
            if (std::meta::is_function(_member)
                && !is_operator_function(_member)
                && has_identifier(_member)
                && std::meta::identifier_of(_member) == "invoke")
                return _member;
        throw "unable to find the invoke function";
    }
}
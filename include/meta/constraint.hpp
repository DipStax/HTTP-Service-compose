#include <meta>

#include <algorithm>

namespace extra::meta
{
    template<std::meta::info InterfaceType>
    constexpr bool is_interface()
    {
        constexpr auto ctx = std::meta::access_context::unchecked();
        constexpr auto members = define_static_array(std::meta::members_of(InterfaceType, ctx));

        for (std::meta::info member : members) {
            if (is_special_member_function(member))
                continue;
            if (is_nonstatic_data_member(member) || !std::meta::is_pure_virtual(member))
                return false;
        }
        return true;
    }

    template<std::meta::info AbstractType>
    consteval bool is_abstraction()
    {
        if (is_interface<AbstractType>())
            return false;

        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto members = define_static_array(std::meta::nonstatic_data_members_of(AbstractType, ctx));

        return std::ranges::count_if(members, std::meta::is_pure_virtual) > 0;
    }

    template<std::meta::info Type>
    consteval bool has_default_constructor()
    {
        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto members = define_static_array(std::meta::members_of(Type, ctx));
        size_t count = 0;

        template for (constexpr std::meta::info _info : members)
            if (is_special_member_function(_info) && is_constructor(_info) && is_default_constructor(_info))
                return true;
        return false;
    }
}
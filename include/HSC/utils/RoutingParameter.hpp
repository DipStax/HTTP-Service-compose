#pragma once

#include <vector>
#include <string>

namespace hsc
{
    template<const char *Route>
    struct RoutingParameter
    {
        static consteval bool IsValid();

        static_assert(IsValid(), "The route is invalid");

        static consteval size_t CountParameters();

        static constexpr size_t params_size = CountParameters();

        static consteval std::array<std::pair<const char *, size_t>, params_size> GetParameters();

        static constexpr std::array<std::pair<const char *, size_t>, params_size> params_name = GetParameters();

        static consteval bool MatchParameter(const std::string_view &_param);
    };
}

#include "HSC/utils/RoutingParameter.inl"
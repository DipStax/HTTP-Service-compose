#pragma once

#include <meta>

#include "HTTP/Method.hpp"

namespace meta::http
{
    template<std::meta::info Func>
    [[nodiscard]] consteval std::tuple<::http::Method, std::string_view> extract_route_type();

    [[nodiscard]] consteval bool has_http_unique_annotation(std::meta::info _func);

    template<std::meta::info Func, std::meta::info Annotation>
    [[nodiscard]] consteval std::optional<std::meta::info> get_unique_route();
}

#include "meta/http.inl"
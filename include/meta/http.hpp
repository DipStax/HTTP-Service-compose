#pragma once

#include <meta>

#include "HTTP/Method.hpp"

#include "meta/metaconcept.hpp"

namespace meta::http
{
    /// @brief Check if a function has unique HTTP annotation
    /// @param _func Function to check on
    /// @return True if the function has unique HTTP annotation, otherwise false.
    [[nodiscard]] consteval bool has_http_unique_annotation(std::meta::info _func);

    /// @brief Get the HTTP annotation, for a specific type, of a function
    /// @tparam Func Function to get the HTTP annotation from
    /// @tparam Annotation HTTP annotation type of the function
    /// @return The route annotation if one. It throw an exception if it has more than one HTTP annotation
    template<std::meta::info Func, std::meta::info Annotation>
        requires IsMetaFunction<Func> && IsMetaType<Annotation>
    [[nodiscard]] consteval std::optional<std::meta::info> get_unique_route();

    /// @brief Get the route method and it's path
    /// @tparam Func Function to extract from
    /// @return The method and the path of the route
    template<std::meta::info Func>
        requires IsMetaFunction<Func>
    [[nodiscard]] consteval std::tuple<::http::Method, std::string_view> extract_route_type();
}

#include "meta/http.inl"
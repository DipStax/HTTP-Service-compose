#include "meta/http.hpp"

namespace meta::http
{
    consteval bool has_http_unique_annotation(std::meta::info _func)
    {
        std::vector<std::meta::info> http_get = annotations_of(_func, ^^::http::Get);
        std::vector<std::meta::info> http_post = annotations_of(_func, ^^::http::Post);
        size_t total = http_get.size() + http_post.size();

        if (total > 1)
            throw "HTTP route should be single";
        else if (total == 0)
            return false;
        else
            return true;
    }

    template<std::meta::info Func, std::meta::info Annotation>
        requires IsMetaFunction<Func> && IsMetaType<Annotation>
    consteval std::optional<std::meta::info> get_unique_route()
    {
        std::vector<std::meta::info> annotations = annotations_of(Func, Annotation);

        switch (annotations.size()) {
            case 0: return std::nullopt;
            case 1: return annotations[0];
            // in the futur, use a compile time exception (can't use a static_assert because annotations can't be constexpr)
            default: throw ("HTTP route should be unique for a function");
        }
    }

    template<std::meta::info Func>
        requires IsMetaFunction<Func>
    consteval std::tuple<::http::Method, std::string_view> extract_route_type()
    {
        std::optional<std::meta::info> get_route = get_unique_route<Func, ^^::http::Get>();
        std::optional<std::meta::info> post_route = get_unique_route<Func, ^^::http::Post>();

        if (get_route.has_value())
            return std::make_tuple(::http::Method::GET, extract<::http::Get>(get_route.value()).m_route);
        else if (post_route.has_value())
            return std::make_tuple(::http::Method::POST, extract<::http::Get>(post_route.value()).m_route);
        else
            throw "Internal error: unable to determine the type of route";
    }
}
#pragma once

#include "HTTP/Method.hpp"

#define HttpGet(_url) [[=http::Get(std::define_static_string(_url))]]
#define HttpPost(_url) [[=http::Post(std::define_static_string(_url))]]

namespace http
{
    /// @brief Base implementation of a route
    /// @tparam Type HTTP method of the route
    template<Method Type>
    struct Route
    {
        constexpr Route(const char *_route)
            : m_route(_route)
        {
        }

        static constexpr Method type = Type;
        const char *m_route;
    };

    /// @brief GET HTTP route
    struct Get : Route<Method::GET>
    {
        constexpr Get(const char *_route)
            : Route<Method::GET>(_route)
        {
        }
    };

    /// @brief POST HTTP route
    struct Post : Route<Method::POST>
    {
        constexpr Post(const char *_route)
            : Route<Method::POST>(_route)
        {
        }
    };
}
#pragma once

#include "HTTP/Method.hpp"

#define CREATE_ROUTE_STRUCTURE(_name, _type)    \
    struct _name : Route<_type>                 \
    {                                           \
        constexpr _name(const char *_route)     \
            : Route<_type>(_route)              \
        {                                       \
        }                                       \
    };

namespace http
{
    /// @brief Base implementation of a route
    /// @tparam Type HTTP method of the route
    template<Method Type>
    struct Route
    {
        // can't be implemented in Route.inl, otherwise it will be not detected by the macro
        constexpr Route(const char *_route)
            : m_route(_route)
        {
        }

        static constexpr Method type = Type;
        const char *m_route;
    };

    /// @brief CONNECT HTTP route annotation
    CREATE_ROUTE_STRUCTURE(Connect, Method::CONNECT)
    #define HttpConnect(_route) [[=http::Connect(std::define_static_string(_route))]]

    /// @brief DELETE HTTP route annotation
    CREATE_ROUTE_STRUCTURE(Delete, Method::DELETE)
    #define HttpDelete(_route) [[=http::Delete(std::define_static_string(_route))]]

    /// @brief GET HTTP route annotation
    CREATE_ROUTE_STRUCTURE(Get, Method::GET)
    #define HttpGet(_route) [[=http::Get(std::define_static_string(_route))]]

    /// @brief HEAD HTTP route annotation
    CREATE_ROUTE_STRUCTURE(Head, Method::HEAD)
    #define HttpHead(_route) [[=http::Head(std::define_static_string(_route))]]

    /// @brief OPTIONS HTTP route annotation
    CREATE_ROUTE_STRUCTURE(Options, Method::OPTIONS)
    #define HttpOptions(_route) [[=http::Options(std::define_static_string(_route))]]

    /// @brief PATCH HTTP route annotation
    CREATE_ROUTE_STRUCTURE(Patch, Method::PATCH)
    #define HttpPatch(_route) [[=http::Patch(std::define_static_string(_route))]]

    /// @brief POST HTTP route annotation
    CREATE_ROUTE_STRUCTURE(Post, Method::POST)
    #define HttpPost(_route) [[=http::Post(std::define_static_string(_route))]]

    /// @brief PUT HTTP route annotation
    CREATE_ROUTE_STRUCTURE(Put, Method::PUT)
    #define HttpPut(_route) [[=http::Put(std::define_static_string(_route))]]

    /// @brief TRACE HTTP route annotation
    CREATE_ROUTE_STRUCTURE(Trace, Method::TRACE)
    #define HttpTrace(_route) [[=http::Trace(std::define_static_string(_route))]]
}

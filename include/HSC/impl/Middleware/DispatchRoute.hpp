// #pragma once

// #include <functional>

// #include "HTTP/Context.hpp"

// using MiddlewareCallback = std::function<void(http::Context &)>;

// namespace hsc::impl
// {
//     class DispatchRoute
//     {
//         public:
//             DispatchRoute(MiddlewareCallback _next, std::shared_ptr<IRouteProvider> &_route_provider)
//                 : m_next(_next), m_route_provider(_route_provider)
//             {
//             }
//             ~DispatchRoute() = default;

//             void invoke(http::Context &_context)
//             {
//                 m_route_provider->dispatch(_context);
//             }

//         private:
//             std::shared_ptr<IRouteProvider> m_route_provider;
//             MiddlewareCallback m_next;
//     }
// }
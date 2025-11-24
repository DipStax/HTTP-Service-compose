#include "Registery/Route.hpp"

ARegisteredRoute::ARegisteredRoute(const std::string_view _route, http::Method _method)
    : m_route(_route), m_method(_method)
{
}

bool ARegisteredRoute::match(http::Method _method, const std::string &_path) const
{
    return _method == m_method && _path == m_route;
}

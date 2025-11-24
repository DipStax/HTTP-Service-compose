#include "Registery/Route.hpp"

ARegisteredRoute::ARegisteredRoute(const std::string_view _route, http::Method _type)
    : m_route(_route), m_type(_type)
{
}

bool ARegisteredRoute::match(http::Method _method, const std::string &_path) const
{
    return _method == m_method && _path == m_route;
}

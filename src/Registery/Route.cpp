#include "Registery/Route.hpp"

ARegisteredRoute::ARegisteredRoute(const std::string_view _route, http::Method _type)
    : m_route(_route), m_type(_type)
{
}
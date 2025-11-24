#include "HSC/Registery/Service.hpp"

namespace hsc
{
    AService::AService(ServiceType _type, std::string_view _interface)
        : m_type(_type), m_interface(_interface)
    {
    }
}
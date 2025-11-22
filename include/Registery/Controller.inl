#include "Registery/Controller.hpp"

template<class ControllerType>
RegisteredController<ControllerType>::RegisteredController(ControllerCtor _ctor)
    : m_ctor(_ctor)
{
}

template<class ControllerType>
std::shared_ptr<ControllerType> RegisteredController<ControllerType>::create()
{
    return m_ctor();
}
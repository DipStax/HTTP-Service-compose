#pragma once

#include <functional>

template<class ControllerType>
struct RegisteredController
{
    using Controller = ControllerType;
    using ControllerCtor = std::function<Controller()>;

    RegisteredController(ControllerCtor _ctor)
        : m_ctor(_ctor)
    {
    }

    ControllerCtor m_ctor;
};
#pragma once

#include "HSC/utils/ServiceCtorInfo.hpp"

namespace hsc
{
    template<class Implementation>
    using ControllerCtorInfo = ServiceCtorInfo<Implementation, 0>;
}
#pragma once

namespace hsc
{
    /// @brief Different service type
    enum ServiceType
    {
        Singleton,
        Transient,
        Scoped,
    };
}
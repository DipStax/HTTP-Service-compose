#pragma once

#include <string>
#include <memory>

#include "HSC/ScopedContainer.hpp"
#include "HSC/impl/Service/interface/IServiceProvider.hpp"

#include "HTTP/Method.hpp"

namespace http
{
    /// @brief HTTP context of a request
    struct Context
    {
        Method method;                                                              ///< Method of the request
        std::string path;                                                           ///< Route path of the request
        std::shared_ptr<hsc::impl::IServiceProvider> service_provider = nullptr;    ///< ServiceProvider service
        hsc::ScopedContainer scoped_container{};                                    ///< Scoped container of the request
    };
}
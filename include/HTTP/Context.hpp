#pragma once

#include <string>
#include <memory>

#include "HSC/impl/Service/interface/AServiceProvider.hpp"

#include "HTTP/Method.hpp"

namespace http
{
    /// @brief HTTP context of a request
    struct Context
    {
        Method method;                                                              ///< Method of the request
        std::string path;                                                           ///< Route path of the request
        std::shared_ptr<hsc::impl::AServiceProvider> service_provider = nullptr;    ///< ServiceProvider service
    };
}
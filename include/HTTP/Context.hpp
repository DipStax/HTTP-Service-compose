#pragma once

#include <string>
#include <memory>

#include "HSC/ScopedContainer.hpp"
#include "HSC/impl/Service/interface/IServiceProvider.hpp"

#include "HTTP/Method.hpp"

namespace http
{
    struct Context
    {
        Method method;
        std::string path;
        hsc::ScopedContainer scoped_container{};
        std::shared_ptr<hsc::impl::IServiceProvider> service_provider = nullptr;
    };
}
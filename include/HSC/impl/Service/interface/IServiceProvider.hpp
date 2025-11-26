#pragma once

#include <any>
#include <string_view>

namespace hsc
{
    class AService;

    namespace impl
    {
        class IServiceProvider
        {
            public:
                virtual ~IServiceProvider() = default;

                /// @brief Get the instance of a singleton service
                /// @param _interface Interface identifier of the singleton service to get
                /// @return The shared_ptr to the implementation of the service as any
                [[nodiscard]] virtual std::any getSingletonService(const std::string_view &_interface) const = 0;

                /// @brief Get service creator information
                /// @param _interface Interface identifier of the service
                /// @return The service creator
                [[nodiscard]] virtual const std::shared_ptr<AService> &getServiceInfo(const std::string_view &_interface) const = 0;
        };
    }
}
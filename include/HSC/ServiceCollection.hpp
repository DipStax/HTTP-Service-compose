#pragma once

#include <ranges>

#include "HSC/Registery/Route.hpp"
#include "HSC/ServiceContainer.hpp"

#include "HTTP/Method.hpp"

namespace hsc
{
    class ServiceBuilder;

    /// @brief Runtime collection of route, service creator
    class ServiceCollection
    {
        public:
            ServiceCollection(ServiceBuilder &_service_builder);
            ~ServiceCollection() = default;

            /// @brief Dispatch an request from the server to it's route
            /// @param _method HTTP method of the request
            /// @param _path Route of the request
            void dispatch(http::Method _method, const std::string &_path);

        private:
            std::vector<std::unique_ptr<ARegisteredRoute>> m_registered_routes;     ///< Registered route
            ServiceContainer m_service_container;                                   ///< Container of all the available service
    };
}
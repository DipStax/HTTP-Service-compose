#pragma once

#include <ranges>

#include "ServiceBuilder.hpp"
#include "ServiceContainer.hpp"
#include "ServiceBuilder.hpp"

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
        ServiceContainer m_service_container;                                   ///< Container of all the available service

        std::vector<std::unique_ptr<ARegisteredRoute>> m_registered_routes;     ///< Registered route
};
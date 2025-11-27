#pragma once

#include <ranges>

#include "HSC/Registery/Route.hpp"

#include "HTTP/Method.hpp"

namespace hsc
{
    class ServiceBuilder;

    /// @brief Runtime collection of route, service creator
    class ServiceCollection
    {
        public:
            ServiceCollection(std::shared_ptr<impl::IServiceProvider> _service_provider);
            ~ServiceCollection() = default;

            template<IsMiddleware MW, class ...Args>
            ServiceCollection &addMiddleware(Args &&..._args);

            /// @brief Dispatch an request from the server to it's route
            /// @param _method HTTP method of the request
            /// @param _path Route of the request
            void dispatch(http::Method _method, const std::string &_path);

        private:
            std::shared_ptr<impl::IServiceProvider> m_service_provider;
    };
}
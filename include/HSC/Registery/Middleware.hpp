#pragma once

#include "HSC/impl/Service/interface/IServiceProvider.hpp"
#include "HTTP/Context.hpp"

#include "meta/concept.hpp"
#include "meta/using.hpp"

namespace hsc
{
    class AMiddleware
    {
        public:
            virtual ~AMiddleware() = default;

            /// @brief Create the internal instance of the middleware
            /// @param _cb Callback to the next middleware
            /// @param _service_provider Service provider service
            virtual void create(MiddlewareCallback _cb, std::shared_ptr<impl::IServiceProvider> &_service_provider) = 0;

            /// @brief Call the invoke function on the internal instance of the middleware
            /// @param _ctx HTTP context of the request
            virtual void run(http::Context &_ctx) = 0;
    };

    template<IsMiddleware MW>
    class Middleware : public AMiddleware
    {
        public:
            using MiddlewareType = MW;
            using Ctor = std::function<std::unique_ptr<MiddlewareType>(MiddlewareCallback, std::shared_ptr<impl::IServiceProvider> &)>;
            using InvokeFunc = std::function<void(std::unique_ptr<MiddlewareType> &, http::Context &)>;

            Middleware(Ctor _ctor, InvokeFunc _invoke);
            ~Middleware() = default;

            void create(MiddlewareCallback _cb, std::shared_ptr<impl::IServiceProvider> &_service_provider) override;

            void run(http::Context &_ctx) override;

        private:
            Ctor m_ctor;                                    ///< Constructor callback of the middleware
            InvokeFunc m_invoke;                            ///< Invoke callback function

            std::unique_ptr<MiddlewareType> m_instance;     ///< Internal instance of the middleware
    };
}

#include "HSC/Registery/Middleware.inl"
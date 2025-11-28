#pragma once

#include "HSC/impl/Service/interface/IServiceProvider.hpp"

#include "meta/concept.hpp"
#include "meta/using.hpp"

namespace hsc
{
    class AMiddleware
    {
        public:
            virtual ~AMiddleware() = default;

            virtual void create(MiddlewareCallback _cb, std::shared_ptr<impl::IServiceProvider> &_service_provider) = 0;

            virtual void run(http::Context &_ctx) = 0;
    };

    template<IsMiddleware MW>
    class Middleware : public AMiddleware
    {
        public:
            using MiddlewareType = MW;
            using Ctor = std::function<std::unique_ptr<MiddlewareType>(MiddlewareCallback, std::shared_ptr<impl::IServiceProvider> &)>;
            using InvokeFunc = std::function<void(std::unique_ptr<MiddlewareType> &, http::Context &)>;

            Middleware(Ctor _ctor, InvokeFunc _invoke)
                : m_ctor(_ctor), m_invoke(_invoke)
            {
            }
            ~Middleware() = default;

            void create(MiddlewareCallback _cb, std::shared_ptr<impl::IServiceProvider> &_service_provider) override
            {
                m_instance = m_ctor(_cb, _service_provider);
            }

            void run(http::Context &_ctx) override
            {
                m_invoke(m_instance, _ctx);
            }

        private:
            Ctor m_ctor;
            InvokeFunc m_invoke;

            std::unique_ptr<MiddlewareType> m_instance;
    };
}
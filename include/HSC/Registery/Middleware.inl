#include "HSC/Registery/Middleware.hpp"

namespace hsc
{
    template<IsMiddleware MW>
    Middleware<MW>::Middleware(Ctor _ctor, InvokeFunc _invoke)
        : m_ctor(_ctor), m_invoke(_invoke)
    {
    }

    template<IsMiddleware MW>
    void Middleware<MW>::create(MiddlewareCallback _cb, std::shared_ptr<impl::AServiceProvider> &_service_provider)
    {
        m_instance = m_ctor(_cb, _service_provider);
    }

    template<IsMiddleware MW>
    void Middleware<MW>::run(http::Context &_ctx)
    {
        m_invoke(m_instance, _ctx);
    }
}
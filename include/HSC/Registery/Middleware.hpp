#pragma once

#include "meta/using.hpp"

namespace hsc
{
    class AMiddleware
    {
        public:
            virtual ~AMiddleware() = default;
    };

    template<IsMiddleware MW>
    class Middleware
    {
        public:
            using MiddlewareType = MW;
            using Ctor = std::function<std::shared_ptr<MiddlewareType>(MiddlewareCallback, std::shared_ptr<impl::IServiceProvider> &)>;

            Middleware(Ctor _ctor);
            ~Middleware() = default;

        private:
            Ctor m_ctor;
    };
}
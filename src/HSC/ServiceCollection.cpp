#include <print>

#include "HSC/impl/Middleware/DispatchRoute.hpp"

#include "HSC/ServiceCollection.hpp"
#include "HSC/ServiceBuilder.hpp"

namespace hsc
{
    ServiceCollection::ServiceCollection(std::shared_ptr<impl::IServiceProvider> _service_provider)
        : m_service_provider(_service_provider)
    {
    }

    void ServiceCollection::dispatch(http::Method _method, const std::string &_path)
    {
        MiddlewareCallback callback = [] (http::Context &_context) { std::ignore = _context; };

        addMiddleware<impl::DispatchRoute>();

        for (auto it = m_registered_middlewares.rbegin(); it != m_registered_middlewares.rend(); it++) {
            (*it)->create(callback, m_service_provider);
            callback = [it] (http::Context &_ctx) {
                (*it)->run(_ctx);
            };
        }
        http::Context context{ _method, _path };
        m_registered_middlewares[0]->run(context);
    }
}
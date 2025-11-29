#include <print>
#include <functional>
#include <ranges>
#include <iostream>
#include <any>

#include "meta/using.hpp"
#include "HTTP/Response.hpp"

#define SERVICE_INTERFACE_NAMESPACE service_imp

namespace service_imp
{
    struct IAuthService
    {
        virtual ~IAuthService() = default;

        virtual void auth() = 0;
    };

    struct IUpdateService
    {
        virtual ~IUpdateService() = default;

        virtual void update() = 0;
    };

    struct IDummyService
    {
        virtual ~IDummyService() = default;

        virtual void dummy() = 0;
    };

    struct UpdateService : IUpdateService
    {
        UpdateService()
        {
            std::println("[ctor] SERVICE update");
        }

        void update()
        {
            std::println("update");
        }
    };

    struct AuthService : IAuthService
    {
        AuthService(std::shared_ptr<IUpdateService> _update)
        {
            std::println("[ctor] SERVICE Auth");
            _update->update();
        }

        void auth() override
        {
            std::println("auth");
        }
    };

    struct DummyService : IDummyService
    {
        DummyService(std::shared_ptr<IAuthService> _auth)
        {
            std::println("[ctor] SERVICE dummy");
            _auth->auth();
        }

        void dummy() override
        {
            std::println("dummy");
        }
    };
}

namespace middleware_imp
{
    struct Middleware
    {
        Middleware(hsc::MiddlewareCallback _cb)
            : m_cb(_cb)
        {
        }

        http::Response invoke(http::Context &_ctx, std::shared_ptr<service_imp::IAuthService> _auth)
        {
            std::println("[Middleware] logging before next");
            _auth->auth();
            m_cb(_ctx);
            std::println("[Middleware] logging after next");
            return http::Response{};
        }

        hsc::MiddlewareCallback m_cb;
    };
}

#include "HSC/ServiceBuilder.hpp"

struct ILogger
{
    virtual void log(const std::string &_log) = 0;
};

namespace controller_imp
{
    struct [[=hsc::controller::Basic()]] Default
    {
        Default(std::shared_ptr<service_imp::IDummyService> _service)
        {
            std::println("[ctor] CONTROLLER Default");
            _service->dummy();
        }

        std::string m_value;

        HttpGet("/api/v1")
        http::Response GetDefault()
        {
            std::println("ROUTE /api/v1");
            return http::Response{};
        }
    };

    // struct [[=controller::Basic()]] WithDIOn
    // {
    //     WithDIOn(ILogger &_logger)
    //     : m_logger(_logger)
    //     {
    //     }

    //     ILogger &m_logger;
    // };
}




int main(int _ac, char **_av)
{
    std::println("[main] Initializing services");
    hsc::ServiceBuilder builder{};

    builder.addScoped<service_imp::IDummyService, service_imp::DummyService>();
    builder.addScoped<service_imp::IAuthService, service_imp::AuthService>();
    builder.addScoped<service_imp::IUpdateService, service_imp::UpdateService>();

    std::println("[main] Adding controllers");
    builder.addController<^^controller_imp>();

    try {
        std::println("[main] Building service collection");
        hsc::ServiceCollection services = builder.build();

        services.addMiddleware<middleware_imp::Middleware>();

        std::println("[main] Disptaching an event");
        services.dispatch(http::Method::GET, "/api/v1");
    } catch (const hsc::ControllerDIException &_ex) {
        std::println("{}", _ex.what());
    } catch (const char *_ex) {
        std::println("error: {}", _ex);
    }
    return 0;
}
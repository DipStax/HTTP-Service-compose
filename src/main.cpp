#include <print>
#include <functional>
#include <ranges>
#include <iostream>
#include <any>

#define SERVICE_INTERFACE_NAMESPACE service_imp

namespace service_imp
{
    struct IAuthService
    {
        virtual void auth() = 0;
    };

    struct IUpdateSerivce
    {
        virtual void update() = 0;
    };

    struct AuthService : IAuthService
    {
        AuthService(std::shared_ptr<IUpdateSerivce> _auth)
        {
            std::println("[ctor] SERVICE Auth");
        }

        void auth() override
        {
            std::println("auth");
        }
    };

    struct UpdateService : IUpdateSerivce
    {
        void update()
        {
            std::println("update");
        }
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
        Default(std::shared_ptr<service_imp::IAuthService> _service)
        {
            std::println("[ctor] CONTROLLER Default");
            _service->auth();
        }

        std::string m_value;

        HttpGet("/api/v1")
        void GetDefault()
        {
            std::println("ROUTE /api/v1");
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
    hsc::ServiceBuilder builder{};

    builder.addTransient<service_imp::IAuthService, service_imp::AuthService>();
    builder.addTransient<service_imp::IUpdateSerivce, service_imp::UpdateService>();

    builder.addController<^^controller_imp>();

    hsc::ServiceCollection services = builder.build();

    services.dispatch(http::Method::GET, "/api/v1");
    return 0;
}
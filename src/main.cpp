// https://compiler-explorer.com/z/vj7dP4svG
#include <print>
#include <functional>
#include <ranges>
#include <iostream>
#include <any>

#include "ServiceBuilder.hpp"

namespace service
{
    struct Singleton
    {
    };

    struct Scoped
    {
    };
}

struct ILogger
{
    virtual void log(const std::string &_log) = 0;
};

namespace controller_imp
{
    struct [[=controller::Basic()]] Default
    {
        std::string m_value;

        // HttpPost("")
        HttpGet("/api/v1")
        void GetDefault()
        {
            std::println("Hello world");
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

struct IAuthService
{
    virtual void auth() = 0;
};

struct IUpdateSerivce
{
    virtual void update() = 0;
};

namespace service_imp
{
    struct [[=service::Singleton()]] AuthService : IAuthService
    {
        void auth() override
        {
            std::println("ok");
        }
    };

    // namespace v2
    // {
    //     struct [[=service::Transcient()]] AuthService2 : IAuthService
    //     {
    //         void auth()
    //         {
    //             std::println("ok");
    //         }
    //     };
    // }
}




int main(int _ac, char **_av)
{
    ServiceBuilder builder{};

    builder.addSingleton<IAuthService, service_imp::AuthService>();

    for (const std::unique_ptr<IService> &service : builder.m_services) {
        std::println("{}", service->m_identifier);
    }

    builder.addController<^^controller_imp>();

    builder.dispatch("/api/v1");
    // std::println("size of array for service: {}", ServiceCounter::ServiceInNamespace<^^service_imp>());
    // std::println("size of array for controller: {}", ControllerCounter::ControllerInNamespace<^^controller_imp>());

    // for (const ControllerInfo &_info : ControllerBuilder<^^controller_imp>::array) {
    //     std::println("controller: {}", _info.name);
    // }
    return 0;
}
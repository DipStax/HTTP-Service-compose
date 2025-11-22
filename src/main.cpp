// https://compiler-explorer.com/z/vj7dP4svG
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
        AuthService(std::shared_ptr<IUpdateSerivce> _auth, const int number)
        {
        }

        void auth() override
        {
            std::println("ok");
        }
    };

    // namespace v2
    // {
    //     struct AuthService2 : IAuthService
    //     {
    //         void auth()
    //         {
    //             std::println("ok");
    //         }
    //     };
    // }
}


#include "ServiceBuilder.hpp"

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




int main(int _ac, char **_av)
{
    ServiceBuilder builder{};

    builder.addSingleton<service_imp::IAuthService, service_imp::AuthService>(0);

    for (const std::shared_ptr<IService> &service : builder.m_services) {
        std::println("{}", service->m_interface);
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
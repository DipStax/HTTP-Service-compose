# HTTP-Service-compose (HSC)

This repository is a small C++26 experimental library to compose HTTP services, middlewares and controllers with compile-time reflection like ASP.NET.

**Highlights**

- Compile-time discovery of annotated method.
- Runtime route registration produced from compile-time metadata.
- Dependency injection of service into services, controllers and middlewares.

## Requirement

- Compiler: clang/LLVM of [Bloomberg/p2996](https://github.com/bloomberg/clang-p2996)
- CMake: version 3.27

### Ressource

- C++26 [Expansion statement](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p1306r5.html)
- C++26 [Reflection](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p2996r13.html)

## How to use

### Important class

Read more information about `ServiceBuilder`, `ServiceCollection` and the different builtin services and middlewares [here](./doc/Core-class.md)

### Services

The life time is like ASP.NET:

| Consumer \ Injected | Singleton | Scoped | Transient |
|---|---|---|---|
| **Singleton** | **valid** | invalid | invalid |
| **Scoped** | **valid** | **valid** | invalid |
| **Transient** | **valid** | **valid** | **valid** |

The declaration of a service for dependency injection must be a `std::shared_ptr` to the interface of the service you wan't to inject.

> Currently there is no support for different namespace for the same identifier of a service's interface.

### Controllers

As ASP.NET a controller is created for each request and support dependency injection in it's constructor.
A controller is requires to have only one constructor.
In this constructor you can request for dependency injection of singleton, scoped and transient service.

All controller will be search in the namespace you provide as template parameter as follow:
```cpp
service_builder.addControllers<^^your_namespace>();
```
> Currently, it is not possible to use the global namespace as `^^::` to lookup for controller (ICE of bloomberg/p2996)

The controller are find using the annotaion:
```cpp
[[=hsc::controller::Basic()]]
```

They are compose of route function annotated by:
```cpp
[[=http::Post(std::define_static_string("your/route/path"))]]
```

you can simplify it using the integrated macro:
```cpp
HttpPost("your/route/path")
```

Those annotations and macro support the following HTTP method:
- `CONNECT`
- `DELETE`
- `GET`
- `HEAD`
- `OPTIONS`
- `PATCH`
- `POST`
- `PUT`
- `TRACE`

### Middleware

Middleware are like singleton service, meaning they can only take as dependency injection in the constructor singleton services.

They are procide using the `invoke` function using reflection. You can inject any type of services into the invoke function, but it is recommanded to pass the singleton in the constructor for performance reason. It need to be public and have a signature as follow:
```cpp
http::Response invoke(http::Context &_ctx /*, Services injection */);
```

## How to use

Build the project
```shell
mkdir build && cd build
cmake ..
make
```

Then run the example main:
```shell
./hsc_test
```
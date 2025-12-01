# HTTP-Service-compose (HSC)

This repository is a small C++26 experimental library to compose HTTP services, middlewares and controllers with compile-time reflection like ASP.NET.

HSC combines simple lifetime-aware DI, compile-time discovery of controllers, and an extensible middleware pipeline to provide a compact, predictable way to route and handle HTTP-style requests. The framework is designed so that application code focuses on services and controllers, while the runtime consistently manages creation, lifetime and dispatching behavior.

**Highlights**

- Service registration with dependency injection.
- Compile-time discovery of annotated controller.
- Runtime route registration produced from compile-time metadata.
- Dependency injection of service into services, controllers and middlewares.

## Table of Contents

- [Requirement](#requirement)
    - [Ressource](#ressource)
- [How to use](#how-to-use)
    - [Important class](#important-class)
    - [Services](#services)
    - [Controllers](#controllers)
    - [Middlewares](#middlewares)
- [Build](#build)
- [License](#license)
- [Maintainer](#maintainer)

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

The service life time is like ASP.NET:
- **Singleton**: instanciated at the begining of the `ServiceCollection::run`.
- **Scoped**: instanciated for each new request and reuse acrose services and middlewares in the scope of the request.
- **Transient**: isntanciated when ever it is requested from the `ServiceProvider`.

The dependency injection work as follow:

| Consumer \ Injected | Singleton | Scoped | Transient |
|---|---|---|---|
| **Singleton** | **valid** | invalid | invalid |
| **Scoped** | **valid** | **valid** | invalid |
| **Transient** | **valid** | **valid** | **valid** |

The declaration of a service for dependency injection must be a `std::shared_ptr` to the interface of the service you wan't to inject.

> Currently there is no support for different namespace for the same identifier of a service's interface.

### Controllers

As ASP.NET, a controller is created for each request and support dependency injection in it's constructor.
A controller is requires to have only one constructor.
In this constructor you can request for dependency injection of singleton, scoped and transient service.

All controller will be search in the namespace you provide as template parameter when you call `ServiceBuilder::addControllers`:

```cpp
service_builder.addControllers<^^your_namespace>();
```

> Currently, it is not possible to use the global namespace as `^^::` (default template in the implementation) to lookup for controller (ICE on bloomberg/p2996)

To register a new controller, it need to be annotated with the `hsc::controller::Basic` structure:

```cpp
[[=hsc::controller::Basic()]]
```

> In the futur a `Route` annotation will be available to prefix the route in a controller

To register a route in a controller, the function handling the route need to be annotated by a struture of the form `http::Method`, like the following for example with a POST method:

```cpp
[[=http::Post(std::define_static_string("your/route/path"))]]
```

It can be simplified using the macro related to the HTTP method targeted:

```cpp
HttpPost("your/route/path")
```

Those annotations and macro support the following HTTP method:
- `CONNECT`, `DELETE`, `GET`, `HEAD`, `OPTIONS`, `PATCH`, `POST`, `PUT`, `TRACE`

### Middlewares

Middleware are like singleton service, meaning they can only take as dependency injection in the constructor singleton services and they sit between the incoming request and the final handler.
Each middleware receives the request context, Can perform pre-processing and post-processing onto the request and  optionally call the next middleware, or return a response directly.
> `DispatchRoute` is the terminal middleware that actually finds and invokes route handlers.

They are procide using the `invoke` function found using reflection. You can inject any type of services into the invoke function, but it is recommanded to pass all singleton in the constructor for performance reason. It need to be public and have a signature as follow:

```cpp
http::Response invoke(http::Context &_ctx /*, Services injection */);
```

## Build

1. Clone the Repository:

```bash
git clone https://github.com/DipStax/HTTP-Service-compose.git
cd HTTP-Service-Container
```

2. Build with CMake:

```bash
mkdir build && cd build
cmake ..
make
```

3. Run the example main:

```bash
./hsc_test
```

## License

This project is licensed under the [MIT License](LICENSE)

## Maintainer

Maintained by [@DipStax](https://github.com/DipStax)
# HTTP-Service-compose — repository overview

This repository is a small C++26 experimental library to compose HTTP services with compile-time reflection.

**Highlights**
- Compile-time discovery of controllers and annotated methods.
- Runtime route registration produced from compile-time metadata.

## In progress

- Dependency injection with service lifetime.
- Route parameter as route function parameter.
- Route resolution between controller and direct route.

## Requirement

- Compiler: clang/LLVM of [Bloomberg/p2996](https://github.com/bloomberg/clang-p2996)

## Efficiency

### Compile time

- Create all your services in the same namespace and then define `SERVICE_NAMESPACE` to speedup the retreivale of you service implementation at compile time. (the namespace is check recursivly)
> You can't use this feature if any of the service you wan't to use is declared outside of the specified namespace, if so don't declare `SERVICE_NAMESPACE` (it will default to the global namespace) and it will lookup for any type matching the interface. (**currently ICE**)

Recommanded approach:

```cpp
namespace SERVICE_NAMESPACE
{
    // declare your services
}
```

## Run the example

```cpp
clang++ -freflection -fexpansion-statements -fannotation-attributes -fparameter-reflection -stdlib=libc++ -std=c++26 -o test -Iinclude ./src/main.cpp
./test
```
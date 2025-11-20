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

## Run the example

```cpp
clang++ -freflection -fexpansion-statements -fannotation-attributes -fparameter-reflection -stdlib=libc++ -std=c++26 -o test -Iinclude ./src/main.cpp
./test
```
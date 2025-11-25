#pragma once

#include "meta/extra.hpp"

template<class T>
concept IsInterface = meta::extra::is_interface<^^T>();

template<class T>
concept IsAbstraction = meta::extra::is_abstraction<^^T>();

template<class T>
concept IsServiceImplementation = !IsInterface<T> && !IsAbstraction<T>;

template<class T>
concept IsController = !IsInterface<T> && !IsAbstraction<T>;
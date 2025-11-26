#pragma once

#include "meta/extra.hpp"
#include "meta/http.hpp"

template<class T>
concept IsInterface = meta::extra::is_interface<^^T>();

template<class T>
concept IsAbstraction = meta::extra::is_abstraction<^^T>();

template<class T>
concept IsConcret = !IsInterface<T> && !IsAbstraction<T>;

template<class T>
concept AsSingleCtor = meta::extra::count_constructor<^^T>() == 1;

template<class T>
concept IsServiceImplementation = IsConcret<T> && AsSingleCtor<T>;

template<class T>
concept IsController = IsConcret<T> && AsSingleCtor<T>;

template<class T>
concept IsMiddleware = IsConcret<T> && meta::http::has_invoke_function<^^T>();
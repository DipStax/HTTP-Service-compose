#pragma once

#include "meta/extra.hpp"
#include "meta/http.hpp"
#include "meta/using.hpp"

template<class T>
concept IsInterface = meta::extra::is_interface<^^T>();

template<class T>
concept IsAbstraction = meta::extra::is_abstraction<^^T>();

template<class T>
concept IsConcret = !IsInterface<T> && !IsAbstraction<T>;

template<class T>
concept HasSingleCtor = meta::extra::count_constructor<^^T>() == 1;

template<class T>
concept IsServiceImplementation = IsConcret<T> && HasSingleCtor<T>;

template<class T>
concept IsController = IsConcret<T> && HasSingleCtor<T>;

template<class T>
concept IsMiddleware = IsConcret<T> && HasSingleCtor<T> && meta::http::has_invoke_function<^^T>() && meta::extra::is_frist_ctor_parameter<T, ^^hsc::MiddlewareCallback>();
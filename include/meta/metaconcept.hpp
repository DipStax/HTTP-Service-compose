#pragma once

#include <meta>

template<std::meta::info Type>
concept IsMetaType = std::meta::is_type(Type);

template<std::meta::info Namespace>
concept IsMetaNamespace = std::meta::is_namespace(Namespace);

template<std::meta::info Func>
concept IsMetaFunction = std::meta::is_function(Func);

template<std::meta::info Annotation>
concept IsMetaAnnotation = std::meta::is_annotation(Annotation);
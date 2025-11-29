#pragma once

#include "meta/metaconcept.hpp"

namespace meta::extra
{
    ///--------------------------------------------------
    /// Type
    ///--------------------------------------------------

    /// @brief Retreive a type from it's type identifier
    /// @tparam Name Type identifier to retreive
    /// @tparam Namespace Namespace in which to lookup
    /// @return If found, return the type related to the type identifier
    template<const char *Name, std::meta::info Namespace, std::meta::info ...OtherNamespaces>
        requires IsMetaNamespace<Namespace>
    [[nodiscard]] consteval std::optional<std::meta::info> retreive_type();

    ///--------------------------------------------------
    /// Implementation
    ///--------------------------------------------------

    /// @brief Determine if a type is an interface
    /// @tparam T Type to check
    /// @return true if the type is an interface, otherwise false
    template<std::meta::info T>
        requires IsMetaType<T>
    [[nodiscard]] consteval bool is_interface();

    /// @brief Determine if a type is an abstraction
    /// @tparam T Type to check
    /// @return true if the type is an abstraction, otherwise false
    template<std::meta::info T>
        requires IsMetaType<T>
    [[nodiscard]] consteval bool is_abstraction();

    ///--------------------------------------------------
    /// Constructor
    ///--------------------------------------------------

    /// @brief Verify if a type contains a default constructor
    /// @tparam T Type to check on
    /// @return true if the type implement a default constructor, otherwise false
    template<std::meta::info T>
        requires IsMetaType<T>
    [[nodiscard]] consteval bool has_default_constructor();

    /// @brief Count the number of constructor declare in a type
    /// @tparam T Type to count on
    /// @return The number of constructor implemented by the type
    template<std::meta::info T>
        requires IsMetaType<T>
    [[nodiscard]] consteval size_t count_constructor();

    /// @brief Get the first custom constructor implemented by the type
    /// @tparam T Type to query on
    /// @return If found, the function meta info of the constructor
    template<std::meta::info T>
        requires IsMetaType<T>
    [[nodiscard]] consteval std::optional<std::meta::info> get_first_custom_ctor();

    /// @brief Get the default constructor implemented by the type
    /// @tparam T Type to query on
    /// @return If found, the function meta info of the constructor
    template<std::meta::info T>
        requires IsMetaType<T>
    [[nodiscard]] consteval std::optional<std::meta::info> get_default_ctor();

    /// @brief Get a constructor from a type
    /// @tparam T Type to query on
    /// @return If a custom constructor is implemented, the first one to be found is returned, otherwise it's the default constructor
    template<std::meta::info T>
        requires IsMetaType<T>
    [[nodiscard]] consteval std::meta::info get_unique_ctor();

    /// @brief Verfiy that the first parameter of the unique constructor is the template parameter type
    /// @tparam T Class type
    /// @tparam ParamType Parameter type to verify equality on
    /// @return True if the parameter match, otherwise false
    template<class T, std::meta::info ParamType>
        requires IsMetaType<ParamType>
    [[nodiscard]] consteval bool is_first_ctor_parameter();

    ///--------------------------------------------------
    /// Function
    ///--------------------------------------------------

    /// @brief Return the meta info of the function "invoke"
    /// @tparam T Class meta info to query on
    /// @return The function meta info
    template<std::meta::info T>
        requires IsMetaType<T>
    [[nodiscard]] consteval std::meta::info get_invoke_function();
}

#include "meta/extra.inl"
#pragma once
#pragma once

#include <meta>

namespace hsc
{
    namespace controller
    {
        /// @brief Mark a basic controller
        struct Basic
        {
        };
    }

    /// @brief Static class to discover controller in a namespace
    /// @tparam ControllerNamespace Namespace to search for controller in
    template<std::meta::info ControllerNamespace = ^^::>
    struct ControllerDiscovery
    {
        /// @brief Count the number of controller::Basic annotation
        /// @tparam Entity Type to count on
        /// @return Return the number of controller annotation on the type Type
        template<std::meta::info Type>
        [[nodiscard]] static consteval size_t CountControllerAnnotation();

        /// @brief Count recursivly the numer of controller in a namespace
        /// @tparam Namespace Namespace to count in
        /// @return Return the number of controller in the namespace
        template<std::meta::info Namespace>
        [[nodiscard]] static consteval size_t ControllerInNamespace();

        /// @brief Number of controller in the ControllerNamespace namespace
        static constexpr size_t controllers_in_namespace = ControllerInNamespace<ControllerNamespace>();

        /// @brief Recursivly query the controller in the namespace
        /// @tparam Namespace Namespace to query in
        /// @return Array of controller in the namespace
        template<std::meta::info Namespace>
        [[nodiscard]] static consteval std::array<std::meta::info, controllers_in_namespace> QueryController();

        /// @brief List of controller in the ControllerNamespace namespace
        static constexpr std::array<std::meta::info, controllers_in_namespace> controllers_list = QueryController<ControllerNamespace>();
    };
}

#include "HSC/ControllerDiscovery.inl"
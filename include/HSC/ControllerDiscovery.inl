#include "HSC/ControllerDiscovery.hpp"

#include "meta/extra.hpp"

namespace hsc
{
    template<std::meta::info ControllerNamespace>
        requires IsMetaNamespace<ControllerNamespace>
    template<std::meta::info Type>
        requires IsMetaType<Type>
    consteval size_t ControllerDiscovery<ControllerNamespace>::CountControllerAnnotation()
    {
        std::vector<std::meta::info> annotations_controler = annotations_of(Type, ^^controller::Basic);
        // more to come

        return annotations_controler.size();
    }

    template<std::meta::info ControllerNamespace>
        requires IsMetaNamespace<ControllerNamespace>
    template<std::meta::info Namespace>
        requires IsMetaNamespace<Namespace>
    consteval size_t ControllerDiscovery<ControllerNamespace>::ControllerInNamespace()
    {
        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto namespace_info = define_static_array(std::meta::members_of(Namespace, ctx));
        size_t num = 0;

        template for (constexpr std::meta::info namespace_entity : namespace_info) {
            if constexpr (is_namespace(namespace_entity))
                num += ControllerInNamespace<namespace_entity>();
            else if constexpr (is_type(namespace_entity))
                num += CountControllerAnnotation<namespace_entity>();
        }
        return num;
    }

    template<std::meta::info ControllerNamespace>
        requires IsMetaNamespace<ControllerNamespace>
    template<std::meta::info Namespace>
        requires IsMetaNamespace<Namespace>
    consteval std::array<std::meta::info, ControllerDiscovery<ControllerNamespace>::controllers_in_namespace> ControllerDiscovery<ControllerNamespace>::QueryController()
    {
        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto namespace_info = define_static_array(std::meta::members_of(Namespace, ctx));
        std::array<std::meta::info, controllers_in_namespace> controllers{};

        template for (size_t it = 0; constexpr std::meta::info namespace_entity : namespace_info) {
            if constexpr (is_namespace(namespace_entity)) {
                constexpr size_t controller_in_scoped_namespace = ControllerInNamespace<namespace_entity>();
                constexpr std::array<std::meta::info, controller_in_scoped_namespace> scoped_controllers = QueryController<namespace_entity>();

                template for (constexpr std::meta::info controller : scoped_controllers)
                    controllers[it++] = controller;
            } else if constexpr (std::meta::is_type(namespace_entity) && std::meta::is_class_type(namespace_entity)) {
                constexpr size_t count_annotation = CountControllerAnnotation<namespace_entity>();

                if constexpr (count_annotation > 0) {
                    static_assert(count_annotation == 1, "Only one controller annotation is allowed for a controller");
                    static_assert(!meta::extra::is_interface<namespace_entity>(), "Controller cannot be an interface");
                    static_assert(!meta::extra::is_abstraction<namespace_entity>(), "Controller cannot be an abstraction");
                    controllers[it++] = namespace_entity;
                }

            }
        }
        return controllers;
    }
}
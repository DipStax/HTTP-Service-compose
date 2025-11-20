#pragma once
#pragma once

#include <experimental/meta>

#include "meta/constraint.hpp"

namespace controller
{
    struct Basic
    {
    };
}

template<std::meta::info ControllerNamespace = ^^::>
struct ControllerBuilder
{
    template<std::meta::info Entity>
    static consteval size_t CountControllerAnnotation()
    {
        if (std::meta::is_class_type(Entity)) {
            std::vector<std::meta::info> annotations_controler = annotations_of(Entity, ^^controller::Basic);
            size_t size = annotations_controler.size();

            if (size == 1)
                return 1;
            else if (size > 1)
                throw "Only one controller annotation is allowed for a controller";
        }
        return 0;
    }

    template<std::meta::info Namespace>
    static consteval size_t ControllerInNamespace()
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

    static constexpr size_t controllers_in_namespace = ControllerInNamespace<ControllerNamespace>();

    template<std::meta::info Namespace>
    static consteval std::array<std::meta::info, controllers_in_namespace> AddController()
    {
        constexpr auto ctx = std::meta::access_context::current();
        constexpr auto namespace_info = define_static_array(std::meta::members_of(Namespace, ctx));
        std::array<std::meta::info, controllers_in_namespace> controllers{};

        template for (size_t it = 0; constexpr std::meta::info namespace_entity : namespace_info) {
            if constexpr (is_namespace(namespace_entity)) {
                constexpr size_t controller_in_scoped_namespace = ControllerInNamespace<namespace_entity>();
                std::array<std::meta::info, controller_in_scoped_namespace> scoped_controllers = AddService<namespace_entity>();

                template for (constexpr std::meta::info controller : scoped_controllers)
                    controllers[it++] = controller;
            } else if constexpr (is_type(namespace_entity)) {
                if constexpr (CountControllerAnnotation<namespace_entity>()) {
                    if constexpr (extra::meta::is_interface<namespace_entity>())
                        throw "Controller cannot be an interface";
                    if constexpr (extra::meta::is_abstraction<namespace_entity>())
                        throw "Controller cannot be an abstraction";
                    controllers[it++] = namespace_entity;
                }
            }
        }
        return controllers;
    }

    static constexpr std::array<std::meta::info, controllers_in_namespace> controllers_list = AddController<ControllerNamespace>();
};
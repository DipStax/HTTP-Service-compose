#pragma once

#include "HSC/Exception/ServiceDIException.hpp"

namespace hsc
{
    class ControllerDIException : public std::exception
    {
        public:
            ControllerDIException(const std::string &_msg, const std::string_view &_controller, const std::string_view &_interface, std::unique_ptr<ServiceDIException> _inner = nullptr) noexcept;
            ControllerDIException(const ControllerDIException &_ex) noexcept;
            ~ControllerDIException() = default;

            ControllerDIException& operator=(const ControllerDIException& _ex);

            const char *what() const noexcept;

        private:
            void buildBuffer() noexcept;

            std::string m_msg;
            std::string_view m_controller;
            std::string_view m_interface;
            std::unique_ptr<ServiceDIException> m_inner = nullptr;

            std::string m_what_buffer{};
    };
}
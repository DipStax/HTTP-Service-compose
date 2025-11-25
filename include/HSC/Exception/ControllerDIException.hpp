#pragma once

#include "HSC/Exception/ServiceDIException.hpp"

namespace hsc
{
    /// @brief Exception related to the dependency injection on controller
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

            std::string m_msg;                                      ///< Message of the exception
            std::string_view m_controller;                          ///< Controller identifier
            std::string_view m_interface;                           ///< DI Interface identifier
            std::unique_ptr<ServiceDIException> m_inner = nullptr;  ///< Service DI related exception

            std::string m_what_buffer{};                            ///< Buffer for the what function
    };
}
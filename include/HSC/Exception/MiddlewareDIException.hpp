#pragma once

#include "HSC/Exception/ServiceDIException.hpp"

namespace hsc
{
    class MiddlewareDIException : public std::exception
    {
        public:
            MiddlewareDIException(const std::string &_msg, const std::string_view &_middleware, const std::string_view &_interface, std::unique_ptr<ServiceDIException> _inner = nullptr) noexcept;
            MiddlewareDIException(const MiddlewareDIException &_ex) noexcept;
            ~MiddlewareDIException() = default;

            MiddlewareDIException& operator=(const MiddlewareDIException& _ex);

            const char *what() const noexcept;

        private:
            void buildBuffer() noexcept;

            std::string m_msg;                                      ///< Message of the exception
            std::string_view m_middleware;                          ///< Controller identifier
            std::string_view m_interface;                           ///< DI Interface identifier
            std::unique_ptr<ServiceDIException> m_inner = nullptr;  ///< Service DI related exception

            std::string m_what_buffer{};                            ///< Buffer for the what function
    };
}
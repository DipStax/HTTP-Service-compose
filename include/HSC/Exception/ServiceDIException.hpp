#pragma once

#include <exception>
#include <string_view>
#include <memory>

namespace hsc
{
    class ServiceDIException : public std::exception
    {
        public:
            ServiceDIException(const std::string &_msg, const std::string_view &_implementation, const std::string_view &_interface,
                const std::string_view &_target, std::unique_ptr<ServiceDIException> _inner = nullptr) noexcept;
            ServiceDIException(const ServiceDIException &_ex) noexcept;
            ~ServiceDIException() = default;

            ServiceDIException& operator=(const ServiceDIException& _ex);

            const char* what() const noexcept override;

        private:
            void buildBuffer() noexcept;

            std::string m_msg;
            std::string_view m_implementation;
            std::string_view m_interface;
            std::string_view m_target;
            std::unique_ptr<ServiceDIException> m_inner = nullptr;

            std::string m_what_buffer;
    };
}
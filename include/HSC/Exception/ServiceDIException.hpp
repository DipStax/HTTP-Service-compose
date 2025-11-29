#pragma once

#include <memory>

#include "HSC/Exception/ServiceException.hpp"

namespace hsc
{
    /// @brief Exception related to the dependency injection on service
    class ServiceDIException : public ServiceException
    {
        public:
            ServiceDIException(const std::string &_msg, const std::string_view &_interface, const std::string_view &_implementation, const std::string_view &_target, std::unique_ptr<ServiceException> _inner = nullptr) noexcept;
            ServiceDIException(const ServiceDIException &_ex) noexcept;
            ~ServiceDIException() = default;

            ServiceDIException& operator=(const ServiceDIException& _ex);

            const char* what() const noexcept override;

        private:
            void buildBuffer() noexcept;

            std::string m_msg;                                      ///< Message of the exception
            std::string_view m_implementation;                      ///< Current implementation identifier
            std::string_view m_interface;                           ///< Current interface identifier
            std::string_view m_target;                              ///< Target identifier of the dependency injection
            std::unique_ptr<ServiceException> m_inner = nullptr;  ///< Possible inner exception

            std::string m_buffer;                              ///< Buffer for the what function
    };
}
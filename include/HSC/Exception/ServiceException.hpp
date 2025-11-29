#pragma once

#include <exception>
#include <string>
#include <string_view>

namespace hsc
{
    /// @brief Base exception for service creation exception
    class ServiceException : public std::exception
    {
        public:
            ServiceException(const std::string &_msg, const std::string_view &_interface) noexcept;
            ServiceException(const ServiceException &_ex) noexcept;
            virtual ~ServiceException() = default;

            ServiceException operator=(const ServiceException &_ex);

            [[nodiscard]] virtual const char *what() const noexcept;

            /// @brief Get the message of the exception
            /// @return The message of the exception
            [[nodiscard]] const std::string &getMessage() const noexcept;
            /// @brief Get the related interface of the exception
            /// @return Related interface of the exception
            [[nodiscard]] const std::string_view &getInterface() const noexcept;

        private:
            void buildBuffer() noexcept;

            std::string m_msg;              ///< Message of the exception
            std::string_view m_interface;   ///< Related iterface of the exception

            std::string m_buffer{};         ///< Buffer for what function
    };
}
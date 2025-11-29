#pragma once

#include <exception>
#include <string>
#include <string_view>

namespace hsc
{
    class ServiceException : public std::exception
    {
        public:
            ServiceException(const std::string &_msg, const std::string_view &_interface) noexcept;
            ServiceException(const ServiceException &_ex) noexcept;
            virtual ~ServiceException() = default;

            ServiceException operator=(const ServiceException &_ex);

            [[nodiscard]] virtual const char *what() const noexcept;

            [[nodiscard]] const std::string &getMessage() const noexcept;
            [[nodiscard]] const std::string_view &getInterface() const noexcept;

        private:
            void buildBuffer() noexcept;

            std::string m_msg;
            std::string_view m_interface;

            std::string m_buffer{};
    };
}
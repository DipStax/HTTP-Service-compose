#include <format>

#include "HSC/Exception/ServiceException.hpp"

namespace hsc
{
    ServiceException::ServiceException(const std::string &_msg, const std::string_view &_interface) noexcept
        : m_msg(_msg), m_interface(_interface)
    {
        buildBuffer();
    }
    ServiceException::ServiceException(const ServiceException &_ex) noexcept
        : m_msg(_ex.m_msg), m_interface(_ex.m_interface)
    {
        buildBuffer();
    }

    ServiceException ServiceException::operator=(const ServiceException &_ex)
    {
        if (this == &_ex)
            return *this;
        m_msg = _ex.m_msg;
        m_interface = _ex.m_msg;
        buildBuffer();
        return *this;
    }

    const char *ServiceException::what() const noexcept
    {
        return m_buffer.c_str();
    }

    const std::string &ServiceException::getMessage() const noexcept
    {
        return m_msg;
    }

    const std::string_view &ServiceException::getInterface() const noexcept
    {
        return m_interface;
    }

    void ServiceException::buildBuffer() noexcept
    {
        m_buffer.append("Service exception: ");
        m_buffer.append(std::format("[interface: {}]: {}", m_interface, m_msg));
    }

}
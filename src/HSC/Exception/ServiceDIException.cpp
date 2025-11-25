#include <format>

#include "HSC/Exception/ServiceDIException.hpp"

namespace hsc
{
    ServiceDIException::ServiceDIException(const std::string &_msg, const std::string_view &_implementation, const std::string_view &_interface,
        const std::string_view &_target, std::unique_ptr<ServiceDIException> _inner) noexcept
        : m_msg(_msg), m_implementation(_implementation), m_interface(_interface), m_target(_target), m_inner(std::move(_inner))
    {
        buildBuffer();
    }

    ServiceDIException::ServiceDIException(const ServiceDIException &_ex) noexcept
        : m_msg(_ex.m_msg), m_implementation(_ex.m_implementation), m_interface(_ex.m_interface), m_target(_ex.m_target)
    {
        if (_ex.m_inner != nullptr)
            m_inner = std::make_unique<ServiceDIException>(*_ex.m_inner);
        buildBuffer();
    }

    ServiceDIException& ServiceDIException::operator=(const ServiceDIException& _ex) {
        if (this == &_ex)
            return *this;
        m_msg = _ex.m_msg;
        m_implementation = _ex.m_implementation;
        m_interface = _ex.m_interface;
        m_target = _ex.m_target;
        m_implementation = _ex.m_implementation;
        if (_ex.m_inner)
            m_inner = std::make_unique<ServiceDIException>(*_ex.m_inner);
        return *this;
    }

    const char* ServiceDIException::what() const noexcept
    {
        return m_what_buffer.c_str();
    }

    void ServiceDIException::buildBuffer() noexcept
    {
        m_what_buffer.append("Service DI Exception:");

        m_what_buffer.append(std::format(" [interface: '{}']", m_interface));
        m_what_buffer.append(std::format(" [impl: '{}']", m_implementation));
        m_what_buffer.append(std::format(" [target: '{}'] ", m_target));
        m_what_buffer.append(m_msg);

        if (m_inner != nullptr)
        {
            m_what_buffer.append("\n  -> Inner: ");
            m_what_buffer.append(m_inner->what());
        }
    }
}

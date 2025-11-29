#include <format>

#include "HSC/Exception/ServiceDIException.hpp"

namespace hsc
{
    ServiceDIException::ServiceDIException(
        const std::string &_msg,
        const std::string_view &_interface,
        const std::string_view &_implementation,
        const std::string_view &_target,
        std::unique_ptr<ServiceException> _inner
    ) noexcept
        : ServiceException(_msg, _interface), m_implementation(_implementation), m_target(_target), m_inner(std::move(_inner))
    {
        buildBuffer();
    }

    ServiceDIException::ServiceDIException(const ServiceDIException &_ex) noexcept
        : ServiceException(_ex), m_implementation(_ex.m_implementation), m_target(_ex.m_target)
    {
        if (_ex.m_inner != nullptr)
            m_inner = std::make_unique<ServiceException>(*_ex.m_inner);
        buildBuffer();
    }

    ServiceDIException& ServiceDIException::operator=(const ServiceDIException& _ex) {
        if (this == &_ex)
            return *this;
        m_implementation = _ex.m_implementation;
        m_target = _ex.m_target;
        m_implementation = _ex.m_implementation;
        if (_ex.m_inner)
            m_inner = std::make_unique<ServiceException>(*_ex.m_inner);
        return *this;
    }

    const char* ServiceDIException::what() const noexcept
    {
        return m_buffer.c_str();
    }

    void ServiceDIException::buildBuffer() noexcept
    {
        m_buffer.append("Service DI Exception:");

        m_buffer.append(std::format(" [interface: '{}']", getInterface()));
        m_buffer.append(std::format(" [impl: '{}']", m_implementation));
        m_buffer.append(std::format(" [target: '{}'] ", m_target));
        m_buffer.append(getMessage());

        if (m_inner != nullptr)
        {
            m_buffer.append("\n  -> Inner: ");
            m_buffer.append(m_inner->what());
        }
    }
}

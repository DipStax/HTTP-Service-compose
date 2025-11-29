#include <format>

#include "HSC/Exception/ControllerDIException.hpp"

namespace hsc
{
    ControllerDIException::ControllerDIException(
        const std::string &_msg,
        const std::string_view &_controller,
        const std::string_view &_interface,
        std::unique_ptr<ServiceException> _inner
    ) noexcept
        : m_msg(_msg), m_controller(_controller), m_interface(_interface), m_inner(std::move(_inner))
    {
        buildBuffer();
    }

    ControllerDIException::ControllerDIException(const ControllerDIException &_ex) noexcept
        : m_msg(_ex.m_msg), m_controller(_ex.m_controller), m_interface(_ex.m_interface)
    {
        if (_ex.m_inner != nullptr)
            m_inner = std::make_unique<ServiceException>(*_ex.m_inner);
        buildBuffer();
    }

    ControllerDIException& ControllerDIException::operator=(const ControllerDIException& _ex) {
        if (this == &_ex)
            return *this;
        m_msg = _ex.m_msg;
        m_controller = _ex.m_controller;
        m_interface = _ex.m_interface;
        if (_ex.m_inner != nullptr)
            m_inner = std::make_unique<ServiceException>(*_ex.m_inner);
        buildBuffer();
        return *this;
    }

    const char *ControllerDIException::what() const noexcept
    {
        return m_buffer.c_str();
    }

    void ControllerDIException::buildBuffer() noexcept
    {
        m_buffer.append("Controller DI Exception:");

        m_buffer.append(std::format(" [controller: '{}']", m_controller));
        m_buffer.append(std::format(" [instantiation: '{}'] ", m_interface));
        m_buffer.append(m_msg);

        if (m_inner != nullptr) {
            m_buffer.append("\n  ");
            m_buffer.append(m_inner->what());
        }
    }
}
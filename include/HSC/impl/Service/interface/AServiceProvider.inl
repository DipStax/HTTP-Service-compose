#include <format>
#include <meta>
#include <thread>

#include "HSC/Exception/ServiceException.hpp"
#include "HSC/impl/Service/interface/AServiceProvider.hpp"

namespace hsc::impl
{
    template<class T>
    std::shared_ptr<T> AServiceProvider::getService()
    {
        return getService<T>(std::format("{}", std::this_thread::get_id()));
    }

    template<class T>
    std::shared_ptr<T> AServiceProvider::getService(const std::string &_scoped_id)
    {
        constexpr std::string_view identifier = std::meta::identifier_of(std::meta::dealias(^^T));
        ServiceType type = getServiceType(identifier);

        if (type == ServiceType::Singleton) {
            try {
                return std::any_cast<std::shared_ptr<T>>(getSingletonService(identifier));
            } catch (std::bad_any_cast _ex) {
                std::ignore = _ex;

                throw ServiceException("internal error: singleton cast failed", identifier);
            }
        } else if (type == ServiceType::Scoped) {
            try {
                return std::any_cast<std::shared_ptr<T>>(getScopedService(identifier, _scoped_id));
            } catch (std::bad_any_cast _ex) {
                std::ignore = _ex;

                throw ServiceException("internal error: registered scoped cast failed", identifier);
            }
        } else {
            try {
                return std::any_cast<std::shared_ptr<T>>(getTransientService(identifier));
            } catch (std::bad_any_cast _ex) {
                std::ignore = _ex;

                throw ServiceException("internal error: registered transient cast failed", identifier);
            }
        }
    }

    template<class T>
    const std::shared_ptr<AService> &AServiceProvider::getServiceInfo() const
    {
        return getService<T>(std::format("{}", std::this_thread::get_id()));
    }

    template<class T>
    const std::shared_ptr<AService> &AServiceProvider::getServiceInfo(const std::string &_scoped_id)
    {
        constexpr std::string_view identifier = std::meta::identifier_of(std::meta::dealias(^^T));

        return getServiceInfo(identifier, _scoped_id);
    }
}
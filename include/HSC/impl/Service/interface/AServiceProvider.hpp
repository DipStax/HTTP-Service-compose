#pragma once

#include <any>
#include <string_view>

#include "HSC/Registery/ServiceType.hpp"

namespace hsc
{
    class AService;

    namespace impl
    {
        class AServiceProvider
        {
            public:
                virtual ~AServiceProvider() = default;

                template<class T>
                [[nodiscard]] std::shared_ptr<T> getService();
                template<class T>
                [[nodiscard]] std::shared_ptr<T> getService(const std::string &_scoped_id);

                template<class T>
                [[nodiscard]] const std::shared_ptr<AService> &getServiceInfo() const;
                template<class T>
                [[nodiscard]] const std::shared_ptr<AService> &getServiceInfo(const std::string &_scoped_id);

                /// @brief Check if a service with the same type and interface identifier is in the store, using the thread scope id
                /// @param _type Type of the service to look for
                /// @param _interface Interface identifier of the service to look for
                /// @return True if the service was found, otherwise false
                [[nodiscard]] virtual bool contains(ServiceType _type, const std::string_view &_interface) const = 0;
                /// @brief Check if a service with the same type and interface identifier is in the store
                /// @param _type Type of the service to look for
                /// @param _interface Interface identifier of the service to look for
                /// @return True if the service was found, otherwise false
                [[nodiscard]] virtual bool contains(ServiceType _type, const std::string_view &_interface, const std::string &_scope_id) const = 0;
                /// @brief Check if a service with the same interface identifier is in the store, using the thread scope id
                /// @param _interface Interface identifier of the service to look for
                /// @param _scoped_id Id of the scope to look into in the case of a scoped service
                /// @return True if the service was found, otherwise false
                [[nodiscard]] virtual bool contains(const std::string_view &_interface) const = 0;
                /// @brief Check if a service with the same interface identifier is in the store
                /// @param _interface Interface identifier of the service to look for
                /// @param _scoped_id Id of the scope to look into in the case of a scoped service
                /// @return True if the service was found, otherwise false
                [[nodiscard]] virtual bool contains(const std::string_view &_interface, const std::string &_scope_id) const = 0;

                /// @brief Get the type of a service in the store, using the thread scope id
                /// @param _interface Interface identifier of the service
                /// @return The service type if found, otherwise throw an exception
                [[nodiscard]] virtual ServiceType getServiceType(const std::string_view &_interface) const = 0;
                /// @brief Get the type of a service in the store
                /// @param _interface Interface identifier of the service
                /// @param _scoped_id Id of the scope to look into in the case of a scoped service
                /// @return The service type if found, otherwise throw an exception
                [[nodiscard]] virtual ServiceType getServiceType(const std::string_view &_interface, const std::string &_scope_id) const = 0;

                /// @brief Get the instance of a singleton service
                /// @param _interface Interface identifier of the singleton service to get
                /// @return The shared_ptr to the implementation of the service as any
                [[nodiscard]] virtual std::any getSingletonService(const std::string_view &_interface) const = 0;

                /// @brief Get the instance of a scoped service
                /// @param _interface Interface identifier of the scoped service to get
                /// @param _scoped_id Id of the scope to look into
                /// @return The shared_ptr to the implementation of the service as any
                [[nodiscard]] virtual std::any getScopedService(const std::string_view &_interface, const std::string &_scope_id) = 0;

                /// @brief Get the instance of a transient service
                /// @param _interface Interface identifier of the transient service to get
                /// @return The shared_ptr to the implementation of the service as any
                [[nodiscard]] virtual std::any getTransientService(const std::string_view &_interface) = 0;

                /// @brief Get service creator information, using the thread scope id
                /// @param _interface Interface identifier of the service
                /// @return The service creator
                [[nodiscard]] virtual const std::shared_ptr<AService> &getServiceInfo(const std::string_view &_interface) const = 0;
                /// @brief Get service creator information
                /// @param _interface Interface identifier of the service
                /// @param _scoped_id Id of the scope to look into in the case of a scoped service
                /// @return The service creator
                [[nodiscard]] virtual const std::shared_ptr<AService> &getServiceInfo(const std::string_view &_interface, const std::string &_scope_id) const = 0;
        };
    }
}

#include "HSC/impl/Service/interface/AServiceProvider.inl"
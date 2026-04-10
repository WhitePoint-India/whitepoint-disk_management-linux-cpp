
#ifndef DISK_SANITIZATION_INTERFACE_HPP
#define DISK_SANITIZATION_INTERFACE_HPP

#include <string>

class DiskSanitizationInterface {
    public:
        std::string key;
        std::string description;

        virtual ~DiskSanitizationInterface() = default;

    protected:
        DiskSanitizationInterface(std::string key, std::string description)
            : key(std::move(key)), description(std::move(description)) {}
};

#endif // DISK_SANITIZATION_INTERFACE_HPP
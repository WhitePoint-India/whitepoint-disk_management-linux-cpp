
#ifndef DISK_SANITIZATION_INTERFACE_HPP
#define DISK_SANITIZATION_INTERFACE_HPP

#include <string>
#include <disks.hpp>

class DiskSanitizationInterface {
    public:
        [[nodiscard]] const std::string& getKey() const noexcept;

        virtual ~DiskSanitizationInterface() noexcept = default;

        virtual void sanitize(DiskVariant& disk) = 0;

    protected:
        explicit DiskSanitizationInterface(std::string key);

    private:
        std::string key_;
};

#endif // DISK_SANITIZATION_INTERFACE_HPP

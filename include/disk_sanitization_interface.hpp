
#ifndef DISK_SANITIZATION_INTERFACE_HPP
#define DISK_SANITIZATION_INTERFACE_HPP

#include <string>
#include <variant>

#include <ata_disk.hpp>
#include <nvme_disk.hpp>

using DiskVariant = std::variant<ATADisk, NVMeDisk>;

class DiskSanitizationInterface {
    public:
        std::string key;

        virtual ~DiskSanitizationInterface() = default;

        virtual void deleteDisk(DiskVariant& disk) = 0;

    protected:
        DiskSanitizationInterface(std::string key);
};

#endif // DISK_SANITIZATION_INTERFACE_HPP
#ifndef DISKS_H
#define DISKS_H

#include <variant>
#include <disk.hpp>

namespace Disks {

class ATADisk : public Disk {
public:
    ATADisk(
        const std::string& serial,
        const std::string& model,
        const std::string& path,
        const std::string& description,
        unsigned long long size,
        unsigned int sectorSize
    );

    [[nodiscard]] bool isFrozen() const;
    void unfreeze();
};

class NVMeDisk : public Disk {
public:
    NVMeDisk(
        const std::string& serial,
        const std::string& model,
        const std::string& path,
        const std::string& description,
        unsigned long long size,
        unsigned int sectorSize
    );
};

class USBDisk : public Disk {
public:
    USBDisk(
        const std::string& serial,
        const std::string& model,
        const std::string& path,
        const std::string& description,
        unsigned long long size,
        unsigned int sectorSize
    );
};

} // namespace Disks

using DiskVariant = std::variant<Disks::ATADisk, Disks::NVMeDisk, Disks::USBDisk>;

#endif // DISKS_H
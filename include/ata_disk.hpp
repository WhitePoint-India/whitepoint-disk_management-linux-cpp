
#ifndef ATA_DISK_HPP
#define ATA_DISK_HPP

#include <disk.hpp>
#include <ata_disk_sanitization_interface.hpp>

class ATADisk: public Disk {
public:
    using Disk::Disk;

    [[nodiscard]] bool isFrozen();

    void unfreeze();

    void sanitize(ATADiskSanitizationInterface &sanitizationInterface);
};

#endif // ATA_DISK_HPP
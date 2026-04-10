
#ifndef ATA_DISK_HPP
#define ATA_DISK_HPP

#include <disk.hpp>
#include <sanitization_stage.hpp>

class ATADiskSanitizationInterface;

class ATADisk: public Disk {
public:
    using Disk::Disk;

    [[nodiscard]] bool isFrozen() const;

    void unfreeze();

    void sanitize(ATADiskSanitizationInterface &sanitizationInterface, SanitizationCallback callback);
};

#endif // ATA_DISK_HPP

#include <ata_disk.hpp>
#include <ata_disk_sanitization_interface.hpp>

bool ATADisk::isFrozen() const {

    return true;
}

void ATADisk::unfreeze() {

}

void ATADisk::sanitize(ATADiskSanitizationInterface &sanitizationInterface) {
    sanitizationInterface.deleteDisk(*this);
}
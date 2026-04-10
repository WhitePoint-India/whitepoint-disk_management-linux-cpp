
#include <secure_erase_enhanced.hpp>

EnhancedSecureErase::EnhancedSecureErase() : DiskSanitizationInterface("SECURE_ERASE_ENHANCED") {

}

EnhancedSecureErase& EnhancedSecureErase::shared() {
    static EnhancedSecureErase instance;
    return instance;
}

void EnhancedSecureErase::sanitize(DiskVariant& disk, Callback callback) {
    std::visit([this, callback](auto& d) { deleteDisk(d, callback); }, disk);
}

void EnhancedSecureErase::deleteDisk(NVMeDisk& /* disk */, Callback /* callback */) {
    // Implement enhanced secure erase for NVMe disks
}

void EnhancedSecureErase::deleteDisk(ATADisk& /* disk */, Callback /* callback */) {
    // Implement enhanced secure erase for ATA disks
}

#include <secure_erase_enhanced.hpp>

EnhancedSecureErase::EnhancedSecureErase() : DiskSanitizationInterface("SECURE_ERASE_ENHANCED") {

}

EnhancedSecureErase& EnhancedSecureErase::shared() {
    static EnhancedSecureErase instance;
    return instance;
}

void EnhancedSecureErase::deleteDisk(DiskVariant& disk) {
    std::visit([this](auto& d) { deleteDisk(d); }, disk);
}

void EnhancedSecureErase::deleteDisk(NVMeDisk& /* disk */) {
    // Implement enhanced secure erase for NVMe disks
}

void EnhancedSecureErase::deleteDisk(ATADisk& /* disk */) {
    // Implement enhanced secure erase for ATA disks
}
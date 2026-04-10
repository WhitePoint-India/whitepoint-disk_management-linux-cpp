
#include <secure_erase_enhanced.hpp>

EnhancedSecureErase::EnhancedSecureErase()
    : DiskSanitizationInterface("enhanced_secure_erase", "Enhanced Secure Erase") {}

EnhancedSecureErase& EnhancedSecureErase::shared() {
    static EnhancedSecureErase instance;
    return instance;
}

void EnhancedSecureErase::deleteDisk(NVMeDisk& disk) {
    // Implement enhanced secure erase for NVMe disks
}

void EnhancedSecureErase::deleteDisk(ATADisk& disk) {
    // Implement enhanced secure erase for ATA disks
}
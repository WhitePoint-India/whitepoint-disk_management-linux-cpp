
#include <secure_erase.hpp>

SecureErase::SecureErase() : DiskSanitizationInterface("SECURE_ERASE") {

}

SecureErase& SecureErase::shared() {
    static SecureErase instance;
    return instance;
}

void SecureErase::sanitize(DiskVariant& disk) {
    std::visit([this](auto& d) { deleteDisk(d); }, disk);
}

/// @brief
/// @param disk
void SecureErase::deleteDisk(NVMeDisk& /* disk */) {

}

/// @brief
/// @param disk
void SecureErase::deleteDisk(ATADisk& /* disk */) {

}
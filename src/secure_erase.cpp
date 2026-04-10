
#include <secure_erase.hpp>

SecureErase::SecureErase() : DiskSanitizationInterface("SECURE_ERASE") {

}

SecureErase& SecureErase::shared() {
    static SecureErase instance;
    return instance;
}

void SecureErase::sanitize(DiskVariant& disk, Callback callback) {
    std::visit([this, callback](auto& d) { deleteDisk(d, callback); }, disk);
}

/// @brief
/// @param disk
/// @param callback
void SecureErase::deleteDisk(NVMeDisk& /* disk */, Callback /* callback */) {

}

/// @brief
/// @param disk
/// @param callback
void SecureErase::deleteDisk(ATADisk& /* disk */, Callback /* callback */) {

}
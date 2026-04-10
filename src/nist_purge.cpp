
#include <nist_purge.hpp>

NISTPurge::NISTPurge(): DiskSanitizationInterface("NIST_800_88_PURGE") {}

NISTPurge& NISTPurge::shared() {
    static NISTPurge instance;
    return instance;
}

void NISTPurge::sanitize(DiskVariant& disk, Callback callback) {
    std::visit([this, callback](auto& d) { deleteDisk(d, callback); }, disk);
}

/// @brief NIST 800 88 description for disk sanitization of NVMeDisks using NVMe CLI commands
/// @param disk
/// @param callback
void NISTPurge::deleteDisk(NVMeDisk& /* disk */, Callback /* callback */) {

}

/// @brief NIST 800 88 description for disk sanitization of ATADisks using ATA commands
/// @param disk
/// @param callback
void NISTPurge::deleteDisk(ATADisk& /* disk */, Callback /* callback */) {

}
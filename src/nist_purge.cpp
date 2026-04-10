

#include <nist_purge.hpp>

NISTPurge::NISTPurge(): DiskSanitizationInterface("NIST_800_88_PURGE") {}

NISTPurge& NISTPurge::shared() {
    static NISTPurge instance;
    return instance;
}

void NISTPurge::sanitize(DiskVariant& disk) {
    std::visit([this](auto& d) { deleteDisk(d); }, disk);
}

/// @brief NIST 800 88 description for disk sanitization of NVMeDisks using NVMe CLI commands
/// @param disk
void NISTPurge::deleteDisk(NVMeDisk& /* disk */) {

}

/// @brief NIST 800 88 description for disk sanitization of ATADisks using ATA commands
/// @param disk
void NISTPurge::deleteDisk(ATADisk& /* disk */) {

}
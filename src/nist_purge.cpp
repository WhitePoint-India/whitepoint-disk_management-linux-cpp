

#include <nist_purge.hpp>

NISTPurge::NISTPurge()
    : DiskSanitizationInterface("nist_800_88_purge", "NIST 800-88 Purge") {}

NISTPurge& NISTPurge::shared() {
    static NISTPurge instance;
    return instance;
}

/// @brief NIST 800 88 description for disk sanitization of NVMeDisks using NVMe CLI commands
/// @param disk
void NISTPurge::deleteDisk(NVMeDisk& disk) {

}

/// @brief NIST 800 88 description for disk sanitization of ATADisks using ATA commands
/// @param disk
void NISTPurge::deleteDisk(ATADisk& disk) {
    
}
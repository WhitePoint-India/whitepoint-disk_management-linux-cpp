
#include "hw.h"
#include "main.h"


#include <ata_disk.hpp>
#include <nvme_disk.hpp>
#include <nist_clear.hpp>
#include <nist_purge.hpp>
#include <secure_erase.hpp>
#include <secure_erase_enhanced.hpp>

#include <disk_management>
#include <filesystem>
#include <fstream>

constexpr unsigned int kDefaultSectorSize = 512;

// Provide a stub for the status function used by lshw
// In a real implementation, this could show progress to the user
void status(const char* args) {
    // Uncomment for lshw status debugging
    // std::cout << "[lshw]: " << args << std::endl;
}

namespace DiskManagement {

const std::vector<std::reference_wrapper<DiskSanitizationInterface>> methods = {
    NISTClear::shared(),
    NISTPurge::shared(),
    SecureErase::shared(),
    EnhancedSecureErase::shared(),
};

namespace {

// Extracts the block device name from a logical path (e.g. "/dev/sda" -> "sda")
std::string extractDeviceName(const std::string& logicalName) {
    auto pos = logicalName.rfind('/');
    if (pos == std::string::npos) { return logicalName; }
    return logicalName.substr(pos + 1);
}

void fetchDisksRecursively(hwNode* node, hwNode* parent, std::vector<DiskVariant>& disks) {

    if (!node) { return; }

    if (node->getClass() == hw::disk) {

        if (!parent) { return; }

        std::vector<std::string> capabilities = parent->getCapabilitiesList();

        if (capabilities.empty()) {
            std::string nodeDescription = node->getDescription();
            if (nodeDescription.find("NVMe disk") != std::string::npos) {
                capabilities.push_back("nvme");
            }
            else if (nodeDescription.find("ATA Disk") != std::string::npos) {
                capabilities.push_back("sata");
            }
            else {
                return;
            }
        }

        std::string sectorSizeValue = node->getConfig("logicalsectorsize");
        unsigned int sectorSize = sectorSizeValue.empty() ? kDefaultSectorSize : static_cast<unsigned int>(std::stoi(sectorSizeValue));

        for (const std::string& capability: capabilities) {
            if (capability.find("usb") != std::string::npos) {
                return; // USB disks not yet supported as a variant type
            }
            else if (capability.find("nvme") != std::string::npos) {
                std::string logicalName = node->getLogicalName();
                // Filter out hwmon devices (hardware monitoring sensors)
                if (logicalName.find("hwmon") != std::string::npos) {
                    return;
                }
                // Filter out NVMe generic character devices (ng*)
                if (logicalName.find("/dev/ng") == 0) {
                    return;
                }
                disks.emplace_back(NVMeDisk(
                    parent->getSerial(),
                    parent->getProduct(),
                    logicalName,
                    node->getDescription(),
                    node->getSize(),
                    sectorSize
                ));
                return;
            }
            else if (capability.find("sata") != std::string::npos) {
                std::string logicalName = node->getLogicalName();
                disks.emplace_back(ATADisk(
                    node->getSerial(),
                    node->getProduct(),
                    logicalName,
                    node->getDescription(),
                    node->getSize(),
                    sectorSize
                ));
                return;
            }
            else if (capability.find("sas") != std::string::npos) {
                return; // Skip SAS disks for now
            }
            else {
                continue; // Skipping unknown capability.
            }
        }
    }
    else {
        // Recursively check children
        for (unsigned int i = 0; i < node->countChildren(); i++) {
            hwNode* child = node->getChild(i);
            fetchDisksRecursively(child, node, disks);
        }
    }
}

} // anonymous namespace

std::vector<DiskVariant> fetchDisks() {

    // Create a hwNode to scan the system
    hwNode system("computer");

    // Scan the system for hardware
    scan_system(system);

    // Fetch disks
    std::vector<DiskVariant> disks;
    fetchDisksRecursively(&system, nullptr, disks);

    return disks;
}

} // namespace DiskManagement

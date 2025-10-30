
#include "hw.h"
#include "main.h"
#include <iostream>
#include <disk-management>

#define DEFAULT_SECTOR_SIZE 512

// Provide a stub for the status function used by lshw
// In a real implementation, this could show progress to the user
void status(const char* args) {
    // No-op for now
}

namespace {

const DiskManagement::SecureErase secureEraseInstance;
const DiskManagement::GutmannMethod gutmannMethodInstance;

}

const DiskManagement::SecureErase& DiskManagement::SECURE_ERASE_METHOD = secureEraseInstance;
const DiskManagement::GutmannMethod& DiskManagement::GUTMANN_METHOD = gutmannMethodInstance;

namespace {

/**
 * 
 
 std::vector<string> capabilities = parent->getCapabilitiesList();
        for (string& capability: capabilities) {
            if (capability.find("usb") != std::string::npos) {
                std::unique_ptr<DiskManagement::USBDisk> disk = std::make_unique<DiskManagement::USBDisk>(
                    parent->getSerial(),
                    node->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    512
                );
                return disks.push_back(std::move(disk));
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
                std::unique_ptr<DiskManagement::NVMeDisk> disk = std::make_unique<DiskManagement::NVMeDisk>(
                    parent->getSerial(),
                    parent->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    512
                );
                return disks.push_back(std::move(disk));
            }
            else if (capability.find("sata") != std::string::npos) {
                std::unique_ptr<DiskManagement::ATADisk> disk = std::make_unique<DiskManagement::ATADisk>(
                    node->getSerial(),
                    node->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    512,
                    DiskManagement::ATADisk::DiskState::READY
                );
                return disks.push_back(std::move(disk));
            }
            else if (capability.find("sas") != std::string::npos) {
                return; // Skip SAS disks for now
            }
            else {
                continue; // Skipping unknown capability. 
            }
        }
 * 
 * 
 */

void fetchDisksRecursively(hwNode* node, hwNode* parent, std::vector<std::unique_ptr<DiskManagement::Disk>>& disks) {
    
    if (!node) { return; }

    if (node->getClass() == hw::disk) {
        
        std::vector<string> capabilities = parent->getCapabilitiesList();

        if (capabilities.empty()) {
            std::string nodeDescription = node->getDescription();
            if (nodeDescription.find("NVMe disk") != std::string::npos) {
                capabilities.push_back("nvme");
            }
            else if (nodeDescription.find("ATA Disk") != std::string::npos) {
                capabilities.push_back("sata");
            }
            else {
                std::cout << "Unknown disk type for disk: " << node->getLogicalName() << ", description : " << nodeDescription << std::endl;
                return;
            }
        }

        std::string sectorSizeValue = node->getConfig("logicalsectorsize");
        int sectorSize = sectorSizeValue.empty() ? DEFAULT_SECTOR_SIZE : std::stoi(sectorSizeValue);

        for (string& capability: capabilities) {
            if (capability.find("usb") != std::string::npos) {
                std::unique_ptr<DiskManagement::USBDisk> disk = std::make_unique<DiskManagement::USBDisk>(
                    parent->getSerial(),
                    node->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    sectorSize
                );
                return disks.push_back(std::move(disk));
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
                std::unique_ptr<DiskManagement::NVMeDisk> disk = std::make_unique<DiskManagement::NVMeDisk>(
                    parent->getSerial(),
                    parent->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    sectorSize
                );
                return disks.push_back(std::move(disk));
            }
            else if (capability.find("sata") != std::string::npos) {
                std::unique_ptr<DiskManagement::ATADisk> disk = std::make_unique<DiskManagement::ATADisk>(
                    node->getSerial(),
                    node->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    sectorSize,
                    DiskManagement::ATADisk::DiskState::READY
                );
                return disks.push_back(std::move(disk));
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

std::vector<std::unique_ptr<DiskManagement::Disk>> DiskManagement::fetchDisks() {
    // Create a hwNode to scan the system
    hwNode system("computer");
    // Scan the system for hardware
    scan_system(system);
    // Fetch disks using smart pointers
    std::vector<std::unique_ptr<Disk>> disks;
    fetchDisksRecursively(&system, nullptr, disks);
    return disks;
}

#include "hw.h"
#include "main.h"
#include <iostream>
#include <disk-management>

// Provide a stub for the status function used by lshw
// In a real implementation, this could show progress to the user
void status(const char* args) {
    // Uncomment for lshw status debugging
    // std::cout << "[lshw]: " << args << std::endl;
}

namespace {

const DiskManagement::SecureErase secureEraseInstance;
const DiskManagement::GutmannMethod gutmannMethodInstance;

}

const DiskManagement::SecureErase& DiskManagement::SECURE_ERASE_METHOD = secureEraseInstance;
const DiskManagement::GutmannMethod& DiskManagement::GUTMANN_METHOD = gutmannMethodInstance;

namespace {

void fetchDisksRecursively(hwNode* node, hwNode* parent, std::vector<std::unique_ptr<DiskManagement::Disk>>& disks) {

    if (!node) {
        std::cerr << "[DEBUG] fetchDisksRecursively exiting. Node value passed is NULL." << std::endl;
        return;
    }

    if (node->getClass() == hw::disk) {

        std::vector<string> capabilities;

        if (capabilities.empty()) {
            std::string logicalName = node->getLogicalName();
            if (logicalName.find("/dev/nvme") != std::string::npos) {
                std::cerr << "[DEBUG] Identified disk to be NVME. (" << logicalName << ")" << std::endl;
                capabilities.push_back("nvme");
            } else if (logicalName.find("/dev/sd") != std::string::npos) {
                std::cerr << "[DEBUG] Identified disk to be ATA. (" << logicalName << ")" << std::endl;
                capabilities.push_back("sata");
            } else if (logicalName.find("/dev/hd") != std::string::npos) {
                std::cerr << "[DEBUG] Identified disk to be IDE. (" << logicalName << ")" << std::endl;
                capabilities.push_back("ide");
            }
        }

        std::string sectorSizeValue = node->getConfig("logicalsectorsize");

        std::cout << "[DEBUG] Sector size of disk '" << node->getLogicalName() << "' :: " << sectorSizeValue << std::endl;

        for (string& capability: capabilities) {
            if (capability.find("usb") != std::string::npos) {
                std::unique_ptr<DiskManagement::USBDisk> disk = std::make_unique<DiskManagement::USBDisk>(
                    parent->getSerial(),
                    node->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    0
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
                // Use alternate: sectorsize
                std::unique_ptr<DiskManagement::NVMeDisk> disk = std::make_unique<DiskManagement::NVMeDisk>(
                    parent->getSerial(),
                    parent->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    0
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
                    0,
                    DiskManagement::ATADisk::DiskState::READY
                );
                return disks.push_back(std::move(disk));
            }
            else if (capability.find("sas") != std::string::npos) {
                std::unique_ptr<DiskManagement::Disk> disk = std::make_unique<DiskManagement::Disk>(
                    node->getSerial(),
                    node->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    0
                );
                return disks.push_back(std::move(disk));
            }
            else {
                // Skipping unknown capability. 
                continue;
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
    std::cerr << "[DEBUG] Starting disk detection..." << std::endl;

    // Create a hwNode to scan the system
    hwNode system("computer");
    std::cerr << "[DEBUG] Created hwNode" << std::endl;

    // Scan the system for hardware
    scan_system(system);
    std::cerr << "[DEBUG] Completed scan_system, children count: " << system.countChildren() << std::endl;

    // Fetch disks using smart pointers
    std::vector<std::unique_ptr<Disk>> disks;
    fetchDisksRecursively(&system, nullptr, disks);

    std::cerr << "[DEBUG] Found " << disks.size() << " disks total" << std::endl;
    return disks;
}
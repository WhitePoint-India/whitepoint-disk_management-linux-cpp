
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
        std::cerr << "[DEBUG] Null node encountered" << std::endl;
        return;
    }

    // Debug: Show what we're examining
    std::cerr << "[DEBUG] Examining node: class=" << node->getClass()
              << ", product=" << node->getProduct()
              << ", logical=" << node->getLogicalName() << std::endl;

    if (node->getClass() == hw::disk) {
        std::cerr << "[DEBUG] Found disk node!" << std::endl;

        if (!parent) {
            std::cerr << "[ERROR] Disk node has null parent, skipping" << std::endl;
            return;
        }

        std::vector<string> capabilities = parent->getCapabilitiesList();
        std::cerr << "[DEBUG] Parent capabilities count: " << capabilities.size() << std::endl;
        for (const auto& cap : capabilities) {
            std::cerr << "[DEBUG]   Parent capability: " << cap << std::endl;
        }

        // Also check node's own capabilities as fallback
        std::vector<string> nodeCapabilities = node->getCapabilitiesList();
        std::cerr << "[DEBUG] Node capabilities count: " << nodeCapabilities.size() << std::endl;
        for (const auto& cap : nodeCapabilities) {
            std::cerr << "[DEBUG]   Node capability: " << cap << std::endl;
        }

        // If parent has no capabilities, use node's capabilities
        if (capabilities.empty() && !nodeCapabilities.empty()) {
            std::cerr << "[DEBUG] Using node capabilities instead of parent" << std::endl;
            capabilities = nodeCapabilities;
        }

        std::string sectorSizeValue = node->getConfig("logicalsectorsize");
        std::cerr << "[DEBUG] Sector size: '" << sectorSizeValue << "'" << std::endl;

        // If still no capabilities, try to detect from logical name
        if (capabilities.empty()) {
            std::string logicalName = node->getLogicalName();
            std::cerr << "[DEBUG] No capabilities found, checking logical name: " << logicalName << std::endl;

            if (logicalName.find("/dev/nvme") != std::string::npos) {
                capabilities.push_back("nvme");
            } else if (logicalName.find("/dev/sd") != std::string::npos) {
                capabilities.push_back("sata");
            } else if (logicalName.find("/dev/hd") != std::string::npos) {
                capabilities.push_back("ide");
            }
        }

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
                    std::stoi(sectorSizeValue)
                );
                return disks.push_back(std::move(disk));
            }
            else if (capability.find("sata") != std::string::npos || capability.find("ide") != std::string::npos || capability.find("ata") != std::string::npos) {

                if (sectorSizeValue.empty()) {
                    // Try alternative sector size configs
                    sectorSizeValue = node->getConfig("sectorsize");
                    if (sectorSizeValue.empty()) {
                        // Default to 512 if not available but disk has size
                        if (node->getSize() > 0) {
                            sectorSizeValue = "512";
                            std::cerr << "[DEBUG] Using default sector size 512" << std::endl;
                        } else {
                            // Ignoring any connected device for which sectors are not available. eg: CD/DVD ROM.
                            std::cerr << "[DEBUG] Skipping device without sector size and zero size" << std::endl;
                            return;
                        }
                    }
                }

                std::unique_ptr<DiskManagement::ATADisk> disk = std::make_unique<DiskManagement::ATADisk>(
                    node->getSerial(),
                    node->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    std::stoi(sectorSizeValue),
                    DiskManagement::ATADisk::DiskState::READY
                );
                std::cerr << "[DEBUG] Created ATA/SATA/IDE disk" << std::endl;
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
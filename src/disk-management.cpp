#include <disk-management>
#include "hw.h"
#include "main.h"

#include <algorithm>
#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <iostream>

// Provide a stub for the status function used by lshw
// In a real implementation, this could show progress to the user
void status(const char*) {
    // No-op for now
}

namespace {
    const DiskManagement::SecureErase secureEraseInstance;
    const DiskManagement::GutmannMethod gutmannMethodInstance;
}

const DiskManagement::SecureErase& DiskManagement::SECURE_ERASE_METHOD = secureEraseInstance;
const DiskManagement::GutmannMethod& DiskManagement::GUTMANN_METHOD = gutmannMethodInstance;

enum class DiskType {
    NVMe,
    USB,
    SATA,
    PATA,
    MMC,
    VirtIO,
    VirtXen,
    NotADisk,
    Indeterminate
};

namespace {

DiskType determineDiskType(hwNode* disk) {
    const std::string logicalName = disk->getLogicalName();
    
    // Filter out non-block devices
    if (logicalName.empty()) {
        return DiskType::Indeterminate;
    }
    
    // Filter out hwmon devices (hardware monitoring sensors)
    if (logicalName.find("hwmon") != std::string::npos) {
        return DiskType::NotADisk;
    }
    
    // Filter out NVMe generic character devices (ng*)
    if (logicalName.find("/dev/ng") == 0) {
        return DiskType::NotADisk;
    }
    
    // Filter out loop devices (virtual block devices)
    if (logicalName.find("/dev/loop") == 0) {
        return DiskType::NotADisk;
    }
    
    // NVMe block devices: /dev/nvme*
    if (logicalName.find("/dev/nvme") == 0) {
        return (logicalName.find("n1") != std::string::npos) 
            ? DiskType::NVMe 
            : DiskType::NotADisk;
    }
    
    // SATA/SCSI devices: /dev/sd*
    if (logicalName.find("/dev/sd") == 0) {
        const std::string busInfo = disk->getBusInfo();

        std::cout << "BUS INFO: " << busInfo << std::endl;
        return (busInfo.find("usb") != std::string::npos) 
            ? DiskType::USB 
            : DiskType::SATA;
    }
    
    // IDE/PATA devices: /dev/hd*
    if (logicalName.find("/dev/hd") == 0) {
        return DiskType::PATA;
    }
    
    // MMC/SD cards: /dev/mmcblk*
    if (logicalName.find("/dev/mmcblk") == 0) {
        return DiskType::MMC;
    }
    
    // Virtual devices: /dev/vd* (virtio)
    if (logicalName.find("/dev/vd") == 0) {
        return DiskType::VirtIO;
    }
    
    // Xen virtual devices: /dev/xvd*
    if (logicalName.find("/dev/xvd") == 0) {
        return DiskType::VirtXen;
    }
    
    return DiskType::Indeterminate;
}

void fetchDisksRecursively(hwNode* node, hwNode* parent, std::vector<std::unique_ptr<DiskManagement::Disk>>& disks) {
    
    if (!node) { return; }

    if (node->getClass() == hw::disk) {
        std::vector<string> capabilities = parent->getCapabilitiesList();
        for (string& capability: capabilities) {
            if (capability.find("usb") != std::string::npos) {
                auto disk = std::make_unique<DiskManagement::USBDisk>(
                    parent->getSerial(),
                    node->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    0
                );
                disks.push_back(std::move(disk));
                return;
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
                auto disk = std::make_unique<DiskManagement::NVMeDisk>(
                    parent->getSerial(),
                    parent->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    0
                );
                disks.push_back(std::move(disk));
                return;
            }
            else if (capability.find("sata") != std::string::npos) {
                auto disk = std::make_unique<DiskManagement::ATADisk>(
                    node->getSerial(),
                    node->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    0,
                    DiskManagement::ATADisk::DiskState::READY
                );
                disks.push_back(std::move(disk));
                return;
            }
            else if (capability.find("sas") != std::string::npos) {
                auto disk = std::make_unique<DiskManagement::Disk>(
                    node->getSerial(),
                    node->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    0
                );
                disks.push_back(std::move(disk));
                return;
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
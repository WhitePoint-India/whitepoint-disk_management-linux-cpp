#include <disk-management>
#include "internal/secure-erase.h"
#include "internal/gutmann-method.h"
#include "hw.h"
#include "main.h"
#include <functional>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <exception>

#include <iostream>

// Provide a stub for the status function used by lshw
// In a real implementation, this could show progress to the user
void status(const char*) {
    // No-op for now
}

namespace {
    const DiskManagement::Internal::SecureErase secureEraseInstance;
    const DiskManagement::Internal::GutmannMethod gutmannMethodInstance;
}

const DiskManagement::DiskDeleteMethod& DiskManagement::SECURE_ERASE_METHOD = secureEraseInstance;
const DiskManagement::DiskDeleteMethod& DiskManagement::GUTMANN_METHOD = gutmannMethodInstance;

typedef enum _DiskType {
    NVMe,
    USB,
    SATA,
    PATA,
    MMC,
    VirtIO,
    VirtXen,
    not_a_disk,
    indeterminate,
} DiskType;

DiskType diskType(hwNode* disk) {
    std::string logicalName = disk->getLogicalName();
    // Filter out non-block devices
    if (logicalName.empty()) return DiskType::indeterminate;
    // Filter out hwmon devices (hardware monitoring sensors)
    if (logicalName.find("hwmon") != std::string::npos) return DiskType::not_a_disk;
    // Filter out NVMe generic character devices (ng*)
    if (logicalName.find("/dev/ng") == 0) return DiskType::not_a_disk;
    // Filter out loop devices (virtual block devices)
    if (logicalName.find("/dev/loop") == 0) return DiskType::not_a_disk;
    // NVMe block devices: /dev/nvme*
    if (logicalName.find("/dev/nvme") == 0) {
        if (logicalName.find("n1") != std::string::npos) {
            return DiskType::NVMe;
        }
        else {
            return DiskType::not_a_disk;
        }
    }
    // SATA/SCSI devices: /dev/sd*
    if (logicalName.find("/dev/sd") == 0) {
        std::string busInfo = disk->getBusInfo();
        if (busInfo.find("usb") != std::string::npos) {
            return DiskType::USB;
        }
        else {
            return DiskType::SATA;
        }
    }
    // IDE/PATA devices: /dev/hd*
    if (logicalName.find("/dev/hd") == 0) return DiskType::PATA;
    // MMC/SD cards: /dev/mmcblk*
    if (logicalName.find("/dev/mmcblk") == 0) return DiskType::MMC;
    // Virtual devices: /dev/vd* (virtio)
    if (logicalName.find("/dev/vd") == 0) return DiskType::VirtIO;
    // Xen virtual devices: /dev/xvd*
    if (logicalName.find("/dev/xvd") == 0) return DiskType::VirtXen;
    // Unknown
    return DiskType::indeterminate;
}

void fetchDisks(hwNode* node, hwNode* parent, std::vector<DiskManagement::Disk>& disks) {

    if (!node) { return; }

    if (node->getClass() == hw::disk) {

        DiskType type = diskType(node);
        
        // Skip non-disk devices
        if (type == DiskType::not_a_disk) { return; }
        
        // Get disk information from the appropriate node (parent for NVMe, node for others)
        std::string serial;
        std::string vendor;
        std::string product;
        std::string logicalName = node->getLogicalName();
        std::string description;
        std::string busInfo;
        unsigned long long size = 0;
        unsigned long long sectorCount = 0;
        unsigned long long sectorSize = 512; // Default sector size
        
        // Check for duplicates
        for (const auto& existingDisk : disks) {
            if (existingDisk.path == logicalName) {
                return; // Already processed this disk
            }
        }
        
        switch (type) {
            case NVMe: {
                // For NVMe, get serial/vendor/product from parent (controller)
                // but get size/capacity from the node itself (namespace/disk)
                if (parent) {
                    serial = parent->getSerial();
                    vendor = parent->getVendor();
                    product = parent->getProduct();
                }
                // Get size info from the disk node itself
                size = node->getSize();
                sectorCount = node->getCapacity();
                description = node->getDescription();
                
                // Get logical sector size from configuration
                std::string sectorSizeStr = node->getConfig("logicalsectorsize");
                std::cout << sectorSizeStr << "CHECK" << std::endl;
                if (sectorSizeStr.empty()) {
                    std::cout << "1" << std::endl;
                    sectorSizeStr = node->getConfig("sectorsize");
                }
                if (sectorSizeStr.empty()) {
                    std::cout << "2" << std::endl;
                    sectorSizeStr = node->getConfig("logicalblocksize");  
                }
                
                if (!sectorSizeStr.empty()) {
                    std::cout << "Fetched sector size from config: '" << sectorSizeStr << "'" << std::endl;
                    try {
                        sectorSize = std::stoull(sectorSizeStr);
                    } catch (...) {
                        sectorSize = 512;
                        std::cout << "Failed to parse, using default: 512 bytes" << std::endl;
                    }
                } else {
                    // If no config, default to 512
                    std::cout << "No sector size config found, using default: 512 bytes" << std::endl;
                    sectorSize = 512;
                }
                
                std::cout << logicalName << " : NVMe SSD";
                break;
            }
            case USB: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                sectorCount = node->getCapacity();
                
                // Get logical sector size from configuration
                std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (...) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                std::cout << logicalName << " : USB Storage";
                break;
            }
            case SATA: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                sectorCount = node->getCapacity();
                
                // Get logical sector size from configuration
                std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (...) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                std::cout << logicalName << " : SATA/SCSI Disk";
                break;
            }
            case PATA: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                sectorCount = node->getCapacity();
                
                // Get logical sector size from configuration
                std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (...) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                std::cout << logicalName << " : IDE/PATA Disk";
                break;
            }
            case MMC: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                sectorCount = node->getCapacity();
                
                // Get logical sector size from configuration
                std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (...) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                std::cout << logicalName << " : MMC/SD Card";
                break;
            }
            case VirtIO: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                sectorCount = node->getCapacity();
                
                // Get logical sector size from configuration
                std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (...) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                std::cout << logicalName << " : Virtual Disk (VirtIO)";
                break;
            }
            case VirtXen: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                sectorCount = node->getCapacity();
                
                // Get logical sector size from configuration
                std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (...) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                std::cout << logicalName << " : Virtual Disk (Xen)";
                break;
            }
            case indeterminate:
            default: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                sectorCount = node->getCapacity();
                
                // Get logical sector size from configuration
                std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (...) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                std::cout << logicalName << " : Unknown Disk Type";
                break;
            }
        }
        
        // Create and add disk info
        DiskManagement::Disk diskInfo(
            serial,
            product,
            logicalName,
            description,
            size,
            sectorSize,
            DiskManagement::DiskState::READY
        );
        
        disks.push_back(diskInfo);
    }
    else {
        // Recursively check children
        for (unsigned int i = 0; i < node->countChildren(); i++) {
            hwNode* child = node->getChild(i);
            fetchDisks(child, node, disks);
        }
    }
}

std::vector<DiskManagement::Disk> DiskManagement::fetchDisks() {

    // Create a hwNode to scan the system
    hwNode system("computer");
    
    // Scan the system for hardware
    scan_system(system);

    // Fetch disks
    std::vector<DiskManagement::Disk> disks;

    fetchDisks(&system, NULL, disks);

    // Fetch all available disks and return
    return disks;
}
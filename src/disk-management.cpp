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
    if (!node) {
        return;
    }

    if (node->getClass() == hw::disk) {

        const DiskType type = determineDiskType(node);
        
        // Skip non-disk devices
        if (type == DiskType::NotADisk) {
            return;
        }
        
        // Initialize disk information
        std::string serial;
        std::string vendor;
        std::string product;
        const std::string logicalName = node->getLogicalName();
        std::string description;
        unsigned long long size = 0;
        unsigned long long sectorSize = 512; // Default sector size
        
        // Check for duplicates
        auto isDuplicate = std::any_of(disks.begin(), disks.end(),
            [&logicalName](const auto& disk) {
                return disk->path == logicalName;
            });
        
        if (isDuplicate) {
            return;
        }
        
        switch (type) {
            case DiskType::NVMe: {
                // For NVMe, get serial/vendor/product from parent (controller)
                // but get size/capacity from the node itself (namespace/disk)
                if (parent) {
                    serial = parent->getSerial();
                    vendor = parent->getVendor();
                    product = parent->getProduct();
                }
                // Get size info from the disk node itself
                size = node->getSize();
                description = node->getDescription();
                
                // Get logical sector size from configuration
                std::string sectorSizeStr = node->getConfig("logicalsectorsize");
                if (sectorSizeStr.empty()) {
                    sectorSizeStr = node->getConfig("sectorsize");
                }
                if (sectorSizeStr.empty()) {
                    sectorSizeStr = node->getConfig("logicalblocksize");
                }
                
                if (!sectorSizeStr.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeStr);
                    } catch (const std::exception&) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                // Create NVMeDisk object and add to list
                auto nvmeDisk = std::make_unique<DiskManagement::NVMeDisk>(
                    serial,
                    product,
                    logicalName,
                    description,
                    size,
                    sectorSize
                );
                
                disks.push_back(std::move(nvmeDisk));
                return; // Return early since we already added the disk
            }
            case DiskType::USB: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                
                // Get logical sector size from configuration
                const std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (const std::exception&) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                break;
            }
            case DiskType::SATA: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                
                // Get logical sector size from configuration
                const std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (const std::exception&) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                // For SATA disks, create an ATADisk object
                // Default to READY state (could be enhanced to check actual state)
                auto sataDisk = std::make_unique<DiskManagement::ATADisk>(
                    serial,
                    product,
                    logicalName,
                    description,
                    size,
                    sectorSize,
                    DiskManagement::ATADisk::DiskState::READY
                );
                
                disks.push_back(std::move(sataDisk));
                return; // Return early since we already added the disk
            }
            case DiskType::PATA: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                
                // Get logical sector size from configuration
                const std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (const std::exception&) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                // For PATA disks, create an ATADisk object
                // Default to READY state (could be enhanced to check actual state)
                auto pataDisk = std::make_unique<DiskManagement::ATADisk>(
                    serial,
                    product,
                    logicalName,
                    description,
                    size,
                    sectorSize,
                    DiskManagement::ATADisk::DiskState::READY
                );
                
                disks.push_back(std::move(pataDisk));
                return; // Return early since we already added the disk
            }
            case DiskType::MMC: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                
                // Get logical sector size from configuration
                const std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (const std::exception&) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                break;
            }
            case DiskType::VirtIO: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                
                // Get logical sector size from configuration
                const std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (const std::exception&) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                break;
            }
            case DiskType::VirtXen: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                
                // Get logical sector size from configuration
                const std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (const std::exception&) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                break;
            }
            case DiskType::Indeterminate:
            default: {
                serial = node->getSerial();
                vendor = node->getVendor();
                product = node->getProduct();
                description = node->getDescription();
                size = node->getSize();
                
                // Get logical sector size from configuration
                const std::vector<std::string> sectorSizeConfig = node->getConfigValues("logicalsectorsize");
                if (!sectorSizeConfig.empty()) {
                    try {
                        sectorSize = std::stoull(sectorSizeConfig[0]);
                    } catch (const std::exception&) {
                        sectorSize = 512; // Fallback to default
                    }
                }
                
                break;
            }
        }
        
        // Create and add disk info for non-ATA disks
        auto diskInfo = std::make_unique<DiskManagement::Disk>(
            serial,
            product,
            logicalName,
            description,
            size,
            sectorSize
        );
        
        disks.push_back(std::move(diskInfo));
    }
    else {
        // Recursively check children
        for (unsigned int i = 0; i < node->countChildren(); ++i) {
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
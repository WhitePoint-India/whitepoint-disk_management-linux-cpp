#include <disk-management>
#include "internal/secure-erase.h"
#include "internal/gutmann-method.h"
#include "hw.h"
#include "main.h"
#include <functional>
#include <map>

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

std::vector<DiskManagement::Disk> DiskManagement::fetchDisks() {
    std::vector<Disk> disks;
    
    // Create a hwNode to scan the system
    hwNode system("computer");
    
    // Scan the system for hardware
    scan_system(system);
    
    // First pass: collect NVMe controller information
    std::map<std::string, std::string> nvmeSerials; // Maps logical name patterns to serials
    std::map<std::string, std::string> nvmeModels;  // Maps logical name patterns to models
    
    std::function<void(hwNode*)> findNVMeControllers = [&](hwNode* node) {
        // Look for NVMe storage controllers
        if (node->getClass() == hw::storage) {
            std::string logicalName = node->getLogicalName();
            std::string serial = node->getSerial();
            std::string model = node->getProduct();
            
            // If this is an NVMe controller with serial/model info
            if (!logicalName.empty() && logicalName.find("/dev/nvme") != std::string::npos) {
                // Store the serial and model for this controller
                // Extract the nvme number (e.g., nvme0 from /dev/nvme0)
                size_t pos = logicalName.find("nvme");
                if (pos != std::string::npos) {
                    std::string nvmeId = logicalName.substr(pos);
                    if (!serial.empty()) {
                        nvmeSerials[nvmeId] = serial;
                    }
                    if (!model.empty()) {
                        nvmeModels[nvmeId] = model;
                    }
                }
            }
        }
        
        // Recursively check children
        for (unsigned int i = 0; i < node->countChildren(); i++) {
            hwNode* child = node->getChild(i);
            if (child) {
                findNVMeControllers(child);
            }
        }
    };
    
    // Second pass: find disk nodes and match with controller info
    std::function<void(hwNode*)> findDisks = [&](hwNode* node) {
        if (node->getClass() == hw::disk) {
            // Extract disk information
            std::string serial = node->getSerial();
            std::string model = node->getProduct();
            std::string path = node->getLogicalName();
            std::string description = node->getDescription();
            unsigned long long size = node->getSize();
            
            // For NVMe disks, try to get serial/model from the controller
            if (path.find("/dev/nvme") != std::string::npos) {
                // Extract the controller ID (e.g., nvme0 from /dev/nvme0n1)
                size_t pos = path.find("nvme");
                if (pos != std::string::npos) {
                    size_t endPos = path.find("n", pos + 4); // Find 'n' after 'nvme'
                    if (endPos != std::string::npos) {
                        std::string nvmeId = path.substr(pos, endPos - pos);
                        
                        // Use controller serial/model if disk doesn't have them
                        if (serial.empty() && nvmeSerials.find(nvmeId) != nvmeSerials.end()) {
                            serial = nvmeSerials[nvmeId];
                        }
                        if (model.empty() && nvmeModels.find(nvmeId) != nvmeModels.end()) {
                            model = nvmeModels[nvmeId];
                        }
                    }
                }
            }
            
            // Calculate sector count (assuming 512 byte sectors if not specified)
            unsigned long long sectorCount = size > 0 ? size / 512 : 0;
            
            // Default to READY state (would need additional logic to detect FROZEN state)
            DiskState state = DiskState::READY;
            
            // Only add disks with valid path and skip control devices
            if (!path.empty() && path.find("/dev/") != std::string::npos && 
                path.find("ng") == std::string::npos) {  // Skip /dev/ng0n1 type devices
                disks.emplace_back(
                    serial.empty() ? "N/A" : serial,
                    model.empty() ? node->getVendor() : model,
                    path,
                    description.empty() ? "Disk" : description,
                    size,
                    sectorCount,
                    state
                );
            }
        }
        
        // Recursively check children
        for (unsigned int i = 0; i < node->countChildren(); i++) {
            hwNode* child = node->getChild(i);
            if (child) {
                findDisks(child);
            }
        }
    };
    
    // First find NVMe controllers
    findNVMeControllers(&system);
    
    // Then find disks and match with controller info
    findDisks(&system);
    
    return disks;
}
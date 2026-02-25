
#include "hw.h"
#include "main.h"
#include <disks.hpp>
#include <zero_write.hpp>
#include <random_write.hpp>
#include <random_zero_write.hpp>
#include <nist_800_88_clear.hpp>
#include <nist_800_88_purge.hpp>
#include <dod_5220_28_m.hpp>
#include <dod_5220_22_m.hpp>
#include <afssi_5020.hpp>
#include <navso_p5239_26_mfm.hpp>
#include <navso_p5239_26_rll.hpp>
#include <nsa_legacy.hpp>
#include <nsa_modern.hpp>
#include <bsi_vsitr.hpp>
#include <gutmann.hpp>
#include <secure_erase.hpp>
#include <enhanced_secure_erase.hpp>
#include <disk_management>

constexpr unsigned int kDefaultSectorSize = 512;

// Provide a stub for the status function used by lshw
// In a real implementation, this could show progress to the user
void status(const char* args) {
    // Uncomment for lshw status debugging
    // std::cout << "[lshw]: " << args << std::endl;
}

namespace {

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

        for (std::string& capability: capabilities) {
            if (capability.find("usb") != std::string::npos) {
                disks.emplace_back(Disks::USBDisk(
                    parent->getSerial(),
                    node->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    sectorSize
                ));
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
                disks.emplace_back(Disks::NVMeDisk(
                    parent->getSerial(),
                    parent->getProduct(),
                    node->getLogicalName(),
                    node->getDescription(),
                    node->getSize(),
                    sectorSize
                ));
                return;
            }
            else if (capability.find("sata") != std::string::npos) {
                disks.emplace_back(Disks::ATADisk(
                    node->getSerial(),
                    node->getProduct(),
                    node->getLogicalName(),
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

namespace DiskManagement {

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

const std::vector<DiskDeleteMethod*> methods = {
    &ZeroWrite::shared(),
    &RandomWrite::shared(),
    &RandomZeroWrite::shared(),
    &NIST80088Clear::shared(),
    &NIST80088Purge::shared(),
    &DoD522028M::shared(),
    &DoD522022M::shared(),
    &AFSSI5020::shared(),
    &NAVSOP523926MFM::shared(),
    &NAVSOP523926RLL::shared(),
    &NSALegacy::shared(),
    &NSAModern::shared(),
    &BSIVSITR::shared(),
    &Gutmann::shared(),
    &SecureErase::shared(),
    &EnhancedSecureErase::shared()
};

} // namespace DiskManagement
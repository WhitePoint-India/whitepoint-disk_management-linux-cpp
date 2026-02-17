
#include <memory>
#include <iomanip>
#include <iostream>
#include <disk-management>

int main() {
    std::vector<std::unique_ptr<DiskManagement::Disk>> disks = DiskManagement::fetchDisks();
    for (const std::unique_ptr<DiskManagement::Disk>& diskPtr : disks) {
        if (auto ataDisk = dynamic_cast<DiskManagement::ATADisk*>(diskPtr.get())) {
            ataDisk->deleteDisk(DiskManagement::NIST_800_88_CLEAR, [](const double fractionCompleted) {
                std::cout << "ATA NIST_800_88 Progress :: " << fractionCompleted << std::endl;
            });
        }
        else if (auto nvmeDisk = dynamic_cast<DiskManagement::NVMeDisk*>(diskPtr.get())) {
            // nvmeDisk->deleteDisk(DiskManagement::NIST_800_88_CLEAR, [](auto stage, auto& progress) {
            //     std::cout << "NVMe NIST_800_88 Progress :: " << progress.fractionCompleted() << std::endl;
            // });
        }
        else if (auto usbDisk = dynamic_cast<DiskManagement::USBDisk*>(diskPtr.get())) {
            
        }
    }
    return 0;
}

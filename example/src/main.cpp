#include <iomanip>
#include <iostream>
#include <disk-management>
#include <memory>

void callbackATA(DiskOperations::ATADisk::Stage stage, const DiskOperations::Progress& progress) {

    std::cout << "ATA Progress :: " << progress.fractionCompleted() << std::endl;
}

void callbackNVMe(DiskOperations::NVMeDisk::Stage stage, const DiskOperations::Progress& progress) {

    std::cout << "NVMe Progress :: " << progress.fractionCompleted() << std::endl;
}

int main() {
    std::vector<std::unique_ptr<DiskManagement::Disk>> disks = DiskManagement::fetchDisks();
    for (const std::unique_ptr<DiskManagement::Disk>& diskPtr : disks) {
        if (auto ataDisk = dynamic_cast<DiskManagement::ATADisk*>(diskPtr.get())) {
            ataDisk->deleteDisk(DiskOperations::SECURE_ERASE_METHOD, callbackATA);
        }
        else if (auto nvmeDisk = dynamic_cast<DiskManagement::NVMeDisk*>(diskPtr.get())) {
            nvmeDisk->deleteDisk(DiskOperations::SECURE_ERASE_METHOD, callbackNVMe);
        }
    //     else if (auto usbDisk = dynamic_cast<DiskManagement::USBDisk*>(diskPtr.get())) {
    //         // std::cout << "Secure erase not supported for USB disks: " << usbDisk->path << std::endl;
    //     }
    }
    return 0;
}

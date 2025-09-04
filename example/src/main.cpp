#include <iomanip>
#include <iostream>
#include <disk-management>
#include <memory>

int main() {
    
    std::vector<std::unique_ptr<DiskManagement::Disk>> disks = DiskManagement::fetchDisks();
    
    for (const std::unique_ptr<DiskManagement::Disk>& diskPtr : disks) {
        if (auto ataDisk = dynamic_cast<DiskManagement::ATADisk*>(diskPtr.get())) {
            ataDisk->deleteDisk(DiskManagement::SECURE_ERASE_METHOD);
        }
        else if (auto nvmeDisk = dynamic_cast<DiskManagement::NVMeDisk*>(diskPtr.get())) {
            nvmeDisk->deleteDisk(DiskManagement::SECURE_ERASE_METHOD);
        }
        else {
            std::cout << "UNKNOWN DISK" << std::endl;
        }
    }
    
    return 0;
}
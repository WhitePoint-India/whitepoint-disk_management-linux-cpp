#include <iomanip>
#include <iostream>
#include <disk-management>

int main() {
    std::vector<DiskManagement::Disk> disks = DiskManagement::fetchDisks();
    for (DiskManagement::Disk& disk: disks) {
        // Uncomment to actually perform operations (DANGEROUS!)
        disk.deleteDisk(DiskManagement::SECURE_ERASE_METHOD);
    }
    return 0;
}
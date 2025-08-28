
#include <vector>
#include <disk-management>

int main() {
    std::vector<DiskManagement::Disk> disks = DiskManagement::fetchDisks();
    
    for (DiskManagement::Disk& disk : disks) {
        disk.deleteDisk();
        disk.deleteDisk(DiskManagement::GUTMANN_METHOD);
    }
    return 0;
}
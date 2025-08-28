
#include <disk-management/delete.h>
#include <disk-management/details.h>
#include <iostream>

DiskManagement::Disk::Disk() {
    serial = "DUMMY_SERIAL_12345";
    model = "DUMMY_MODEL";
    path = "/dev/sda";
    description = "Dummy disk for testing";
    size = 1099511627776; // 1TB in bytes
    sectorCount = 2147483648; // 2^31 sectors (512 bytes each = 1TB)
    state = DiskState::READY;
}

bool DiskManagement::Disk::isFrozen() {
    return true;
}

void DiskManagement::Disk::unfreeze() {
    std::cout << "Unfreeeeeeeeeeeeeze..." << std::endl;
}

void DiskManagement::Disk::deleteDisk(DiskManagement::DiskDeleteMethod& method) {
    // Call the delete method's deleteDisk function, passing this disk
    method.deleteDisk(*this);
}

DiskManagement::Disk* DiskManagement::fetchDisks() {
    return NULL;
}
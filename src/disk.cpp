
#include <disk-management>

DiskManagement::Disk::Disk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    unsigned long long sectorSize
) : serial(serial),
    model(model), 
    path(path), 
    description(description), 
    size(size), 
    sectorSize(sectorSize) {
}

unsigned long long DiskManagement::Disk::getSectorCount() {
    return size / sectorSize;
}


DiskManagement::ATADisk::ATADisk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    unsigned long long sectorSize,
    DiskState state
) : Disk(
        serial,
        model,
        path,
        description,
        size,
        sectorSize
    ),
    state(state) {
}

bool DiskManagement::ATADisk::isFrozen() {
    return state == DiskState::FROZEN;
}

void DiskManagement::ATADisk::unfreeze() {
    if (state == DiskState::FROZEN) {
        state = DiskState::READY;
    }
}

void DiskManagement::ATADisk::deleteDisk(const ATADiskDeleteMethod& method) {
    method.deleteDisk(*this);
}

void DiskManagement::ATADisk::deleteDisk() {
    SECURE_ERASE_METHOD.deleteDisk(*this);
}

DiskManagement::NVMeDisk::NVMeDisk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    unsigned long long sectorSize
) : Disk(
        serial,
        model,
        path,
        description,
        size,
        sectorSize
    ) {
}

void DiskManagement::NVMeDisk::deleteDisk(const NVMeDiskDeleteMethod& method) {
    method.deleteDisk(*this);
}

void DiskManagement::NVMeDisk::deleteDisk() {
    SECURE_ERASE_METHOD.deleteDisk(*this);
}

DiskManagement::USBDisk::USBDisk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    unsigned long long sectorSize
) : Disk(
        serial,
        model,
        path,
        description,
        size,
        sectorSize
    ) {
}

#include <disk-management>

DiskManagement::Disk::Disk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    int sectorSize
) : serial(serial),
    model(model), 
    path(path), 
    description(description), 
    size(size), 
    sectorSize(sectorSize) {
}

unsigned long long DiskManagement::Disk::getSectorCount() const {
    return size / sectorSize;
}


DiskManagement::ATADisk::ATADisk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    int sectorSize,
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

DiskManagement::NVMeDisk::NVMeDisk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    int sectorSize
) : Disk(
        serial,
        model,
        path,
        description,
        size,
        sectorSize
    ) {
}

DiskManagement::USBDisk::USBDisk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    int sectorSize
) : Disk(
        serial,
        model,
        path,
        description,
        size,
        sectorSize
    ) {
}
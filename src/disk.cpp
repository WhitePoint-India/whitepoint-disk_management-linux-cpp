
#include <disk-management>

DiskManagement::Disk::Disk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    unsigned long long sectorSize,
    DiskState state
) : serial(serial), 
    model(model), 
    path(path), 
    description(description), 
    size(size), 
    sectorSize(sectorSize),
    state(state) {
}

unsigned long long DiskManagement::Disk::getSectorCount() {
    return size / sectorSize;
}

bool DiskManagement::Disk::isFrozen() const {
    return state == DiskState::FROZEN;
}

void DiskManagement::Disk::unfreeze() {
    if (state == DiskState::FROZEN) {
        state = DiskState::READY;
    }
}

void DiskManagement::Disk::deleteDisk(const DiskManagement::DiskDeleteMethod& method) {
    method.deleteDisk(*this);
}
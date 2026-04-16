
#include <ata_disk.hpp>

bool ATADisk::isFrozen() const {

    return true;
}

void ATADisk::unfreeze() {

}

void ATADisk::secureEraseUnit(bool /*enhanced*/) {

}

void ATADisk::writeBlock(uint64_t /*sectorOffset*/, const void* /*data*/, std::size_t /*dataSize*/) {

}

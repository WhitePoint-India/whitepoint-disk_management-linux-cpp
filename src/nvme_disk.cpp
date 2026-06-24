
#include <iostream>
#include <nvme_disk.hpp>

void NVMeDisk::writeBlock(uint64_t /*sectorOffset*/, const void* /*data*/, std::size_t /*dataSize*/) {

}

void NVMeDisk::nvmeSanitize(int sanitizeAction) {

    std::cout << "NVMeSanitize called with action: " << sanitizeAction << std::endl;
}

void NVMeDisk::nvmeFormatNVM() {

}

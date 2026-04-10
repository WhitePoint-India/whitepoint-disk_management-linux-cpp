
#include <nvme_disk.hpp>

void NVMeDisk::sanitize(NVMeDiskSanitizationInterface &sanitizationInterface) {
    sanitizationInterface.deleteDisk(*this);
}
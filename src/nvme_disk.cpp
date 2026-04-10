
#include <nvme_disk.hpp>
#include <nvme_disk_sanitization_interface.hpp>

void NVMeDisk::sanitize(NVMeDiskSanitizationInterface &sanitizationInterface, SanitizationCallback callback) {
    sanitizationInterface.deleteDisk(*this, callback);
}
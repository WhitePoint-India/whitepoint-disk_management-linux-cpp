
#ifndef NVME_DISK_HPP
#define NVME_DISK_HPP

#include <disk.hpp>
#include <sanitization_stage.hpp>

class NVMeDiskSanitizationInterface;

class NVMeDisk: public Disk {
public:
    using Disk::Disk;

    void sanitize(NVMeDiskSanitizationInterface &sanitizationInterface, SanitizationCallback callback);
};

#endif // NVME_DISK_HPP
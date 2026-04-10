
#ifndef NVME_DISK_HPP
#define NVME_DISK_HPP

#include <disk.hpp>

class NVMeDiskSanitizationInterface;

class NVMeDisk: public Disk {
public:
    using Disk::Disk;

    void sanitize(NVMeDiskSanitizationInterface &sanitizationInterface);
};

#endif // NVME_DISK_HPP

#ifndef NVME_DISK_HPP
#define NVME_DISK_HPP

#include <disk.hpp>
#include <nvme_disk_sanitization_interface.hpp>

class NVMeDisk: public Disk {
public:
    
    using Disk::Disk;

    void sanitize(NVMeDiskSanitizationInterface &sanitizationInterface);
};

#endif // NVME_DISK_HPP

#ifndef NVME_DISK_SANITIZATION_INTERFACE_HPP
#define NVME_DISK_SANITIZATION_INTERFACE_HPP

#include <disk_sanitization_interface.hpp>

class NVMeDisk;

class NVMeDiskSanitizationInterface : public virtual DiskSanitizationInterface {
    
    friend class NVMeDisk;

public:
    using DiskSanitizationInterface::sanitize;

protected:
    virtual void deleteDisk(NVMeDisk& disk, Callback callback) = 0;
};

#endif // NVME_DISK_SANITIZATION_INTERFACE_HPP
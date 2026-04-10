

#ifndef ATA_DISK_SANITIZATION_INTERFACE_HPP
#define ATA_DISK_SANITIZATION_INTERFACE_HPP

#include <disk_sanitization_interface.hpp>

class ATADisk;

class ATADiskSanitizationInterface: public virtual DiskSanitizationInterface {
    public:
        using DiskSanitizationInterface::deleteDisk;
        virtual void deleteDisk(ATADisk& disk) = 0;
};

#endif // ATA_DISK_SANITIZATION_INTERFACE_HPP
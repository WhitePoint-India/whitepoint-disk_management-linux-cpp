#ifndef DISKS_H
#define DISKS_H

#include <disk.hpp>
#include <disk-operations.hpp>

namespace DiskManagement {

class Deletable {
public:
    template <typename Self, typename DeleteOperation, typename Delegate>
    void deleteDisk(this Self& self, const DeleteOperation& operation, Delegate& delegate);
};

template <typename Stage>
class ATADiskDeleteOperation; // Forward declaration

template <typename Stage>
class NVMeDiskDeleteOperation; // Forward declaration

class ATADisk : public Disk, public Deletable {
public:
    ATADisk(
        const std::string& serial,
        const std::string& model,
        const std::string& path,
        const std::string& description,
        unsigned long long size,
        int sectorSize
    );

    bool isFrozen();
    void unfreeze();
};

class NVMeDisk : public Disk, public Deletable {
public:
    NVMeDisk(
        const std::string& serial,
        const std::string& model,
        const std::string& path,
        const std::string& description,
        unsigned long long size,
        int sectorSize
    );
};

template <typename Delegate>
class ATADiskDeleteOperation : public AnyDeleteOperation<ATADisk, Delegate> {
};

template <typename Delegate>
class NVMeDiskDeleteOperation : public AnyDeleteOperation<NVMeDisk, Delegate> {
};

class USBDisk : public Disk {
public:
    USBDisk(
        const std::string& serial,
        const std::string& model,
        const std::string& path,
        const std::string& description,
        unsigned long long size,
        int sectorSize
    );
};

} // namespace DiskManagement

#include <disks.tpp>

#endif // DISKS_H

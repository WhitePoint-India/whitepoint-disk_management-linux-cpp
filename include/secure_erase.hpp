
#ifndef SECURE_ERASE_HPP
#define SECURE_ERASE_HPP

#include <ata_disk.hpp>
#include <nvme_disk.hpp>
#include <ata_disk_sanitization_interface.hpp>
#include <nvme_disk_sanitization_interface.hpp>

class SecureErase: public NVMeDiskSanitizationInterface, public ATADiskSanitizationInterface {
    public:
        static SecureErase& shared();

        SecureErase(const SecureErase&) = delete;
        SecureErase(SecureErase&&) = delete;
        SecureErase& operator=(const SecureErase&) = delete;
        SecureErase& operator=(SecureErase&&) = delete;

        void deleteDisk(ATADisk& disk);
        void deleteDisk(NVMeDisk& disk);

    private:
        SecureErase();
};

#endif // SECURE_ERASE_HPP

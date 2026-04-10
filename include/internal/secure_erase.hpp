
#ifndef SECURE_ERASE_HPP
#define SECURE_ERASE_HPP

#include <ata_disk_sanitization_interface.hpp>
#include <nvme_disk_sanitization_interface.hpp>

class SecureErase: public NVMeDiskSanitizationInterface, public ATADiskSanitizationInterface {
    public:
        [[nodiscard]] static SecureErase& shared();

        SecureErase(const SecureErase&) = delete;
        SecureErase(SecureErase&&) = delete;
        SecureErase& operator=(const SecureErase&) = delete;
        SecureErase& operator=(SecureErase&&) = delete;

        void sanitize(DiskVariant& disk, Callback callback) override;

    private:
        SecureErase();
        void deleteDisk(ATADisk& disk, Callback callback) override;
        void deleteDisk(NVMeDisk& disk, Callback callback) override;
};

#endif // SECURE_ERASE_HPP

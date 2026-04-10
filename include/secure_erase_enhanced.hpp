
#ifndef SECURE_ERASE_ENHANCED_HPP
#define SECURE_ERASE_ENHANCED_HPP

#include <ata_disk_sanitization_interface.hpp>
#include <nvme_disk_sanitization_interface.hpp>

class EnhancedSecureErase: public NVMeDiskSanitizationInterface, public ATADiskSanitizationInterface {
    public:
        static EnhancedSecureErase& shared();

        EnhancedSecureErase(const EnhancedSecureErase&) = delete;
        EnhancedSecureErase(EnhancedSecureErase&&) = delete;
        EnhancedSecureErase& operator=(const EnhancedSecureErase&) = delete;
        EnhancedSecureErase& operator=(EnhancedSecureErase&&) = delete;

        void deleteDisk(DiskVariant& disk) override;
        void deleteDisk(ATADisk& disk) override;
        void deleteDisk(NVMeDisk& disk) override;

    private:
        EnhancedSecureErase();
};

#endif // SECURE_ERASE_ENHANCED_HPP


#ifndef SECURE_ERASE_ENHANCED_HPP
#define SECURE_ERASE_ENHANCED_HPP

#include <ata_disk_sanitization_interface.hpp>
#include <nvme_disk_sanitization_interface.hpp>

class EnhancedSecureErase: public NVMeDiskSanitizationInterface, public ATADiskSanitizationInterface {
    public:
        [[nodiscard]] static EnhancedSecureErase& shared();

        EnhancedSecureErase(const EnhancedSecureErase&) = delete;
        EnhancedSecureErase(EnhancedSecureErase&&) = delete;
        EnhancedSecureErase& operator=(const EnhancedSecureErase&) = delete;
        EnhancedSecureErase& operator=(EnhancedSecureErase&&) = delete;
        
        void sanitize(DiskVariant& disk, Callback callback) override;

    private:
        EnhancedSecureErase();
        void deleteDisk(ATADisk& disk, Callback callback) override;
        void deleteDisk(NVMeDisk& disk, Callback callback) override;
};

#endif // SECURE_ERASE_ENHANCED_HPP

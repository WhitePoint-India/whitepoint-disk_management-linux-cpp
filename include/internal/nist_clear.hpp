
#ifndef NIST_CLEAR_HPP
#define NIST_CLEAR_HPP

#include <ata_disk_sanitization_interface.hpp>
#include <nvme_disk_sanitization_interface.hpp>

class NISTClear: public NVMeDiskSanitizationInterface, public ATADiskSanitizationInterface {
    public:
        [[nodiscard]] static NISTClear& shared();

        NISTClear(const NISTClear&) = delete;
        NISTClear(NISTClear&&) = delete;
        NISTClear& operator=(const NISTClear&) = delete;
        NISTClear& operator=(NISTClear&&) = delete;

        void sanitize(DiskVariant& disk) override;

    private:
        NISTClear();
        void deleteDisk(ATADisk& disk) override;
        void deleteDisk(NVMeDisk& disk) override;
};

#endif // NIST_CLEAR_HPP
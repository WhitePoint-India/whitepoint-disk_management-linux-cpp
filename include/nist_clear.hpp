
#ifndef NIST_800_88_CLEAR
#define NIST_800_88_CLEAR

#include <ata_disk_sanitization_interface.hpp>
#include <nvme_disk_sanitization_interface.hpp>

class NISTClear: public NVMeDiskSanitizationInterface, public ATADiskSanitizationInterface {
    public:
        static NISTClear& shared();

        NISTClear(const NISTClear&) = delete;
        NISTClear(NISTClear&&) = delete;
        NISTClear& operator=(const NISTClear&) = delete;
        NISTClear& operator=(NISTClear&&) = delete;

        void deleteDisk(DiskVariant& disk) override;
        void deleteDisk(ATADisk& disk) override;
        void deleteDisk(NVMeDisk& disk) override;

    private:
        NISTClear();
};

#endif // NIST_800_88_CLEAR
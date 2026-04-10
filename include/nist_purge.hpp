
#ifndef NIST_800_88_PURGE
#define NIST_800_88_PURGE

#include <ata_disk.hpp>
#include <nvme_disk.hpp>
#include <ata_disk_sanitization_interface.hpp>
#include <nvme_disk_sanitization_interface.hpp>

class NISTPurge: public NVMeDiskSanitizationInterface, public ATADiskSanitizationInterface {
    public:
        static NISTPurge& shared();

        NISTPurge(const NISTPurge&) = delete;
        NISTPurge(NISTPurge&&) = delete;
        NISTPurge& operator=(const NISTPurge&) = delete;
        NISTPurge& operator=(NISTPurge&&) = delete;

        void deleteDisk(ATADisk& disk);
        void deleteDisk(NVMeDisk& disk);

    private:
        NISTPurge();
};

#endif // NIST_800_88_PURGE
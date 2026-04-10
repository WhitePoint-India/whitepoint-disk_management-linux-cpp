
#ifndef NIST_PURGE_HPP
#define NIST_PURGE_HPP

#include <ata_disk_sanitization_interface.hpp>
#include <nvme_disk_sanitization_interface.hpp>

class NISTPurge: public NVMeDiskSanitizationInterface, public ATADiskSanitizationInterface {
    public:
        [[nodiscard]] static NISTPurge& shared();

        NISTPurge(const NISTPurge&) = delete;
        NISTPurge(NISTPurge&&) = delete;
        NISTPurge& operator=(const NISTPurge&) = delete;
        NISTPurge& operator=(NISTPurge&&) = delete;

        void deleteDisk(DiskVariant& disk) override;
        void deleteDisk(ATADisk& disk) override;
        void deleteDisk(NVMeDisk& disk) override;

    private:
        NISTPurge();
};

#endif // NIST_PURGE_HPP

#include <disks.hpp>

namespace NIST_800_88_Clear {

namespace NVMe {

enum class Stage {
    SETTING_PASSWORD,
    ERASURE,
    VERIFICATION,
    COUNT
};

} // namespace NVMe

namespace ATA {

enum class Stage {
    DISK_FREEZE_CHECK,
    HPA_AVAILABILITY_CHECK,
    HPA_REMOVAL,
    DCO_AVAILABILITY_CHECK,
    DCO_REMOVAL,
    ERASURE,
    VERIFICATION,
    COUNT
};

} // namespace ATA

class DeleteOperation : public DiskManagement::ATADiskDeleteOperation<ATA::Stage>, public DiskManagement::NVMeDiskDeleteOperation<NVMe::Stage>, private Writable {
public:
    void deleteDisk(DiskManagement::NVMeDisk& disk, AnyDeleteOperation<DiskManagement::NVMeDisk, NVMe::Stage>::StageCallback onStage, AnyDeleteOperation<DiskManagement::NVMeDisk, NVMe::Stage>::ProgressCallback onProgress) const override;

    void deleteDisk(DiskManagement::ATADisk& disk, AnyDeleteOperation<DiskManagement::ATADisk, ATA::Stage>::StageCallback onStage, AnyDeleteOperation<DiskManagement::ATADisk, ATA::Stage>::ProgressCallback onProgress) const override;
};

} // namespace NIST_800_88_Clear


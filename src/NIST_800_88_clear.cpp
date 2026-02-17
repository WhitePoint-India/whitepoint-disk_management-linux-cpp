
#include <NIST_800_88_clear.hpp>

namespace NIST_800_88_Clear {

void DeleteOperation::deleteDisk(DiskManagement::ATADisk& disk, AnyDeleteOperation<DiskManagement::ATADisk, ATA::Stage>::StageCallback onStage, AnyDeleteOperation<DiskManagement::ATADisk, ATA::Stage>::ProgressCallback onProgress) const {

    // CHECK FROZEN
    onStage(ATA::Stage::DISK_FREEZE_CHECK);

    // DETECT HPA
    onStage(ATA::Stage::HPA_AVAILABILITY_CHECK);

    // REMOVE HPA
    onStage(ATA::Stage::HPA_REMOVAL);

    // DETECT DCO
    onStage(ATA::Stage::DCO_AVAILABILITY_CHECK);

    // REMOVE DCO
    onStage(ATA::Stage::DCO_REMOVAL);

    // ERASE
    write(disk, Writable::Method::x0, [&](const Progress& progress) {
        onProgress(ATA::Stage::ERASURE, progress);
    });

    // VERIFY
    onStage(ATA::Stage::VERIFICATION);
}

void DeleteOperation::deleteDisk(DiskManagement::NVMeDisk& disk, AnyDeleteOperation<DiskManagement::NVMeDisk, NVMe::Stage>::StageCallback onStage, AnyDeleteOperation<DiskManagement::NVMeDisk, NVMe::Stage>::ProgressCallback onProgress) const {
    for (int i = 0; i <= 100; i++) {
        Progress progress(i, 100);
        onProgress(NVMe::Stage::ERASURE, progress);
    }
}

} // namespace NIST_800_88_Clear



#include <nist_800_88_clear.hpp>

namespace DiskManagement {

NIST80088Clear::NIST80088Clear() : SingletonMethod("NIST_800_88_CLEAR") {

}

const std::string& NIST80088Clear::getTitle() const {
    static const std::string title = "NIST 800-88 CLEAR";
    return title;
}

const std::string& NIST80088Clear::getDescription() const {
    static const std::string description = "NIST 800-88 Clear level sanitization. Applies logical techniques to sanitize data in all user-addressable storage locations.";
    return description;
}

const std::vector<Stage>& NIST80088Clear::getStages() const {
    static const std::vector<Stage> stages = {
        {"zero_overwrite", "Zero overwrite", 0, 2},
        {"verification", "Verification", 1, 2}
    };
    return stages;
}

void NIST80088Clear::execute(Disk& disk, DiskDeleteMethod::Delegate& delegate) {
    const auto& stages = getStages();

    // Stage 1: Zero overwrite
    delegate.onStageStarted(stages[0]);
    write(disk, Method::x0, [&](const Progress& progress) {
        delegate.onProgress(stages[0], progress);
    });
    delegate.onStageCompleted(stages[0]);

    if (delegate.shouldCancel()) return;

    // Stage 2: Verification
    delegate.onStageStarted(stages[1]);
    auto result = verifyFull(disk, [&](const Progress& progress) {
        delegate.onProgress(stages[1], progress);
    });
    delegate.onStageCompleted(stages[1]);

    if (result.passed) {
        delegate.onCompleted();
    } else {
        delegate.onError("Verification failed: " + std::to_string(result.nonZeroSectors) + " non-zero sectors detected");
    }
}

void NIST80088Clear::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }

void NIST80088Clear::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) {
    const auto& stages = getStages();

    // Stage 1: NVMe format command (firmware-based, not a write operation)
    delegate.onStageStarted(stages[0]);
    // TODO: nvme format [disk] -s 1 -f
    delegate.onProgress(stages[0], {disk.getSize(), disk.getSize()});
    delegate.onStageCompleted(stages[0]);

    if (delegate.shouldCancel()) return;

    // Stage 2: Verification
    delegate.onStageStarted(stages[1]);
    auto result = verifyFull(disk, [&](const Progress& progress) {
        delegate.onProgress(stages[1], progress);
    });
    delegate.onStageCompleted(stages[1]);

    if (result.passed) {
        delegate.onCompleted();
    } else {
        delegate.onError("Verification failed: " + std::to_string(result.nonZeroSectors) + " non-zero sectors detected");
    }
}

void NIST80088Clear::deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }

} // namespace DiskManagement


#include <nist_800_88_purge.hpp>

namespace DiskManagement {

NIST80088Purge::NIST80088Purge() : SingletonMethod("NIST_800_88_PURGE") {

}

const std::string& NIST80088Purge::getTitle() const {
    static const std::string title = "NIST 800-88 PURGE";
    return title;
}

const std::string& NIST80088Purge::getDescription() const {
    static const std::string description = "NIST 800-88 Purge level sanitization. Applies physical or logical techniques that render data recovery infeasible using state-of-the-art laboratory techniques.";
    return description;
}

const std::vector<Stage>& NIST80088Purge::getStages() const {
    static const std::vector<Stage> stages = {
        {"hpa_detection", "HPA detection and removal", 0, 4},
        {"dco_detection", "DCO detection and removal", 1, 4},
        {"secure_erase", "Secure erase", 2, 4},
        {"verification", "Verification", 3, 4}
    };
    return stages;
}

void NIST80088Purge::execute(Disk& disk, DiskDeleteMethod::Delegate& delegate) {
    const auto& stages = getStages();

    // Stage 1: HPA detection and removal
    delegate.onStageStarted(stages[0]);
    // TODO: detect and remove Host Protected Area
    delegate.onProgress(stages[0], {disk.getSize(), disk.getSize()});
    delegate.onStageCompleted(stages[0]);

    if (delegate.shouldCancel()) return;

    // Stage 2: DCO detection and removal
    delegate.onStageStarted(stages[1]);
    // TODO: detect and remove Device Configuration Overlay
    delegate.onProgress(stages[1], {disk.getSize(), disk.getSize()});
    delegate.onStageCompleted(stages[1]);

    if (delegate.shouldCancel()) return;

    // Stage 3: Secure erase
    delegate.onStageStarted(stages[2]);
    // TODO: ATA SECURITY ERASE UNIT / NVMe Format / NVMe Sanitize
    delegate.onProgress(stages[2], {disk.getSize(), disk.getSize()});
    delegate.onStageCompleted(stages[2]);

    if (delegate.shouldCancel()) return;

    // Stage 4: Verification
    delegate.onStageStarted(stages[3]);
    auto result = verifyFull(disk, [&](const Progress& progress) {
        delegate.onProgress(stages[3], progress);
    });
    delegate.onStageCompleted(stages[3]);

    if (result.passed) {
        delegate.onCompleted();
    } else {
        delegate.onError("Verification failed: " + std::to_string(result.nonZeroSectors) + " non-zero sectors detected");
    }
}

void NIST80088Purge::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void NIST80088Purge::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void NIST80088Purge::deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }

} // namespace DiskManagement

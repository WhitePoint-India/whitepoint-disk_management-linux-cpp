
#include <nsa_legacy.hpp>

namespace DiskManagement {

NSALegacy::NSALegacy() : SingletonMethod("NSA_LEGACY") {

}

const std::string& NSALegacy::getTitle() const {
    static const std::string title = "NSA LEGACY";
    return title;
}

const std::string& NSALegacy::getDescription() const {
    static const std::string description = "NSA legacy sanitization method using alternating random and zero fill passes for magnetic media.";
    return description;
}

const std::vector<Stage>& NSALegacy::getStages() const {
    static const std::vector<Stage> stages = {
        {"random_pass", "Writing random data", 0, 2},
        {"zero_pass", "Writing zeros", 1, 2}
    };
    return stages;
}

void NSALegacy::execute(Disk& disk, DiskDeleteMethod::Delegate& delegate) {
    const auto& stages = getStages();

    delegate.onStageStarted(stages[0]);
    write(disk, Method::RANDOM, [&](const Progress& progress) {
        delegate.onProgress(stages[0], progress);
    });
    delegate.onStageCompleted(stages[0]);

    if (delegate.shouldCancel()) return;

    delegate.onStageStarted(stages[1]);
    write(disk, Method::x0, [&](const Progress& progress) {
        delegate.onProgress(stages[1], progress);
    });
    delegate.onStageCompleted(stages[1]);
    delegate.onCompleted();
}

void NSALegacy::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void NSALegacy::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void NSALegacy::deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }

} // namespace DiskManagement

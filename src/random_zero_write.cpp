
#include <random_zero_write.hpp>

namespace DiskManagement {

RandomZeroWrite::RandomZeroWrite() : SingletonMethod("RANDOM_ZERO_FILL") {

}

const std::string& RandomZeroWrite::getTitle() const {
    static const std::string title = "RANDOM ZERO WRITE";
    return title;
}

const std::string& RandomZeroWrite::getDescription() const {
    static const std::string description = "Writes random data followed by a zero fill pass to all bytes of the disk.";
    return description;
}

const std::vector<Stage>& RandomZeroWrite::getStages() const {
    static const std::vector<Stage> stages = {
        {"random_pass", "Writing random data", 0, 2},
        {"zero_pass", "Writing zeros", 1, 2}
    };
    return stages;
}

void RandomZeroWrite::execute(Disk& disk, DiskDeleteMethod::Delegate& delegate) {
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

void RandomZeroWrite::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void RandomZeroWrite::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void RandomZeroWrite::deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }

} // namespace DiskManagement


#include <zero_write.hpp>

namespace DiskManagement {

ZeroWrite::ZeroWrite() : SingletonMethod("ZERO_FILL") {

}

const std::string& ZeroWrite::getTitle() const {
    static const std::string title = "ZERO WRITE";
    return title;
}

const std::string& ZeroWrite::getDescription() const {
    static const std::string description = "Writes 0x00 to all bytes of the disk using direct write method.";
    return description;
}

const std::vector<Stage>& ZeroWrite::getStages() const {
    static const std::vector<Stage> stages = {
        {"zero_pass", "Writing zeros", 0, 1}
    };
    return stages;
}

void ZeroWrite::execute(Disk& disk, DiskDeleteMethod::Delegate& delegate) {
    const auto& stages = getStages();

    delegate.onStageStarted(stages[0]);
    write(disk, Method::x0, [&](const Progress& progress) {
        delegate.onProgress(stages[0], progress);
    });
    delegate.onStageCompleted(stages[0]);
    delegate.onCompleted();
}

void ZeroWrite::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void ZeroWrite::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void ZeroWrite::deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }

} // namespace DiskManagement

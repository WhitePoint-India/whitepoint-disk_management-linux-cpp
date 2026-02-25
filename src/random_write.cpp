
#include <random_write.hpp>

namespace DiskManagement {

RandomWrite::RandomWrite() : SingletonMethod("RANDOM_FILL") {

}

const std::string& RandomWrite::getTitle() const {
    static const std::string title = "RANDOM WRITE";
    return title;
}

const std::string& RandomWrite::getDescription() const {
    static const std::string description = "Writes cryptographically random data to all bytes of the disk.";
    return description;
}

const std::vector<Stage>& RandomWrite::getStages() const {
    static const std::vector<Stage> stages = {
        {"random_pass", "Writing random data", 0, 1}
    };
    return stages;
}

void RandomWrite::execute(Disk& disk, DiskDeleteMethod::Delegate& delegate) {
    const auto& stages = getStages();

    delegate.onStageStarted(stages[0]);
    write(disk, Method::RANDOM, [&](const Progress& progress) {
        delegate.onProgress(stages[0], progress);
    });
    delegate.onStageCompleted(stages[0]);
    delegate.onCompleted();
}

void RandomWrite::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void RandomWrite::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void RandomWrite::deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }

} // namespace DiskManagement

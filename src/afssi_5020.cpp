
#include <afssi_5020.hpp>

namespace DiskManagement {

AFSSI5020::AFSSI5020() : SingletonMethod("AFSSI_5020") {

}

const std::string& AFSSI5020::getTitle() const {
    static const std::string title = "AFSSI-5020";
    return title;
}

const std::string& AFSSI5020::getDescription() const {
    static const std::string description = "U.S. Air Force System Security Instruction 5020 3-pass overwrite method for magnetic media sanitization.";
    return description;
}

const std::vector<Stage>& AFSSI5020::getStages() const {
    static const std::vector<Stage> stages = {
        {"pass_1", "Pass 1 - Writing 0x00", 0, 3},
        {"pass_2", "Pass 2 - Writing 0xFF", 1, 3},
        {"pass_3", "Pass 3 - Writing random data", 2, 3}
    };
    return stages;
}

void AFSSI5020::execute(Disk& disk, DiskDeleteMethod::Delegate& delegate) {
    const auto& stages = getStages();

    delegate.onStageStarted(stages[0]);
    write(disk, Method::x0, [&](const Progress& progress) {
        delegate.onProgress(stages[0], progress);
    });
    delegate.onStageCompleted(stages[0]);

    if (delegate.shouldCancel()) return;

    delegate.onStageStarted(stages[1]);
    write(disk, Method::xFF, [&](const Progress& progress) {
        delegate.onProgress(stages[1], progress);
    });
    delegate.onStageCompleted(stages[1]);

    if (delegate.shouldCancel()) return;

    delegate.onStageStarted(stages[2]);
    write(disk, Method::RANDOM, [&](const Progress& progress) {
        delegate.onProgress(stages[2], progress);
    });
    delegate.onStageCompleted(stages[2]);
    delegate.onCompleted();
}

void AFSSI5020::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void AFSSI5020::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void AFSSI5020::deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }

} // namespace DiskManagement

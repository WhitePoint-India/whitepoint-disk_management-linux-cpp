
#include <dod_5220_22_m.hpp>

namespace DiskManagement {

DoD522022M::DoD522022M() : SingletonMethod("DOD_5220_22_M") {

}

const std::string& DoD522022M::getTitle() const {
    static const std::string title = "DoD 5220.22-M";
    return title;
}

const std::string& DoD522022M::getDescription() const {
    static const std::string description = "U.S. Department of Defense 5220.22-M standard 3-pass overwrite method for data sanitization.";
    return description;
}

const std::vector<Stage>& DoD522022M::getStages() const {
    static const std::vector<Stage> stages = {
        {"pass_1", "Pass 1 - Writing 0x00", 0, 3},
        {"pass_2", "Pass 2 - Writing 0xFF", 1, 3},
        {"pass_3", "Pass 3 - Writing random data", 2, 3}
    };
    return stages;
}

void DoD522022M::execute(Disk& disk, DiskDeleteMethod::Delegate& delegate) {
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

void DoD522022M::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void DoD522022M::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void DoD522022M::deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }

} // namespace DiskManagement

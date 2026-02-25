
#include <dod_5220_28_m.hpp>

namespace DiskManagement {

DoD522028M::DoD522028M() : SingletonMethod("DOD_5220_28_M") {

}

const std::string& DoD522028M::getTitle() const {
    static const std::string title = "DoD 5220.28-M";
    return title;
}

const std::string& DoD522028M::getDescription() const {
    static const std::string description = "U.S. Department of Defense 5220.28-M standard 7-pass overwrite method for magnetic media sanitization.";
    return description;
}

const std::vector<Stage>& DoD522028M::getStages() const {
    static const std::vector<Stage> stages = {
        {"pass_1", "Pass 1 - Writing 0x00", 0, 7},
        {"pass_2", "Pass 2 - Writing 0xFF", 1, 7},
        {"pass_3", "Pass 3 - Writing random data", 2, 7},
        {"pass_4", "Pass 4 - Writing 0x00", 3, 7},
        {"pass_5", "Pass 5 - Writing 0xFF", 4, 7},
        {"pass_6", "Pass 6 - Writing random data", 5, 7},
        {"pass_7", "Pass 7 - Verification", 6, 7}
    };
    return stages;
}

void DoD522028M::execute(Disk& disk, DiskDeleteMethod::Delegate& delegate) {
    const auto& stages = getStages();

    constexpr Method writePasses[] = {
        Method::x0, Method::xFF, Method::RANDOM,
        Method::x0, Method::xFF, Method::RANDOM
    };

    for (int i = 0; i < 6; ++i) {
        if (delegate.shouldCancel()) return;

        delegate.onStageStarted(stages[i]);
        write(disk, writePasses[i], [&](const Progress& progress) {
            delegate.onProgress(stages[i], progress);
        });
        delegate.onStageCompleted(stages[i]);
    }

    if (delegate.shouldCancel()) return;

    // Stage 7: Verification (read-through after final random pass)
    delegate.onStageStarted(stages[6]);
    verifyFull(disk, [&](const Progress& progress) {
        delegate.onProgress(stages[6], progress);
    });
    delegate.onStageCompleted(stages[6]);
    delegate.onCompleted();
}

void DoD522028M::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void DoD522028M::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void DoD522028M::deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }

} // namespace DiskManagement

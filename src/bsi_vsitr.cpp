
#include <bsi_vsitr.hpp>

namespace DiskManagement {

BSIVSITR::BSIVSITR() : SingletonMethod("BSI_VSITR") {

}

const std::string& BSIVSITR::getTitle() const {
    static const std::string title = "BSI VSITR";
    return title;
}

const std::string& BSIVSITR::getDescription() const {
    static const std::string description = "German Federal Office for Information Security (BSI) VSITR 7-pass overwrite method with alternating 0x00/0xFF patterns.";
    return description;
}

const std::vector<Stage>& BSIVSITR::getStages() const {
    static const std::vector<Stage> stages = {
        {"pass_1", "Pass 1 - Writing 0x00", 0, 7},
        {"pass_2", "Pass 2 - Writing 0xFF", 1, 7},
        {"pass_3", "Pass 3 - Writing 0x00", 2, 7},
        {"pass_4", "Pass 4 - Writing 0xFF", 3, 7},
        {"pass_5", "Pass 5 - Writing 0x00", 4, 7},
        {"pass_6", "Pass 6 - Writing 0xFF", 5, 7},
        {"pass_7", "Pass 7 - Writing 0xAA", 6, 7}
    };
    return stages;
}

void BSIVSITR::execute(Disk& disk, DiskDeleteMethod::Delegate& delegate) {
    const auto& stages = getStages();

    constexpr Method writePasses[] = {
        Method::x0, Method::xFF,
        Method::x0, Method::xFF,
        Method::x0, Method::xFF
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

    // Pass 7: Write 0xAA
    static constexpr unsigned char patternAA[] = {0xAA};
    delegate.onStageStarted(stages[6]);
    write(disk, patternAA, [&](const Progress& progress) {
        delegate.onProgress(stages[6], progress);
    });
    delegate.onStageCompleted(stages[6]);
    delegate.onCompleted();
}

void BSIVSITR::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void BSIVSITR::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void BSIVSITR::deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }

} // namespace DiskManagement

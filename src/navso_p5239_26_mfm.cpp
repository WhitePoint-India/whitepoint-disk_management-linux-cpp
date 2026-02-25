
#include <navso_p5239_26_mfm.hpp>

namespace DiskManagement {

NAVSOP523926MFM::NAVSOP523926MFM() : SingletonMethod("NAVSO_P5239_26_MFM") {

}

const std::string& NAVSOP523926MFM::getTitle() const {
    static const std::string title = "NAVSO P-5239-26 (MFM)";
    return title;
}

const std::string& NAVSOP523926MFM::getDescription() const {
    static const std::string description = "U.S. Navy NAVSO P-5239-26 3-pass overwrite method for MFM (Modified Frequency Modulation) encoded disks.";
    return description;
}

const std::vector<Stage>& NAVSOP523926MFM::getStages() const {
    static const std::vector<Stage> stages = {
        {"pass_1", "Pass 1 - Writing 0x01", 0, 3},
        {"pass_2", "Pass 2 - Writing 0x27FFFFFF", 1, 3},
        {"pass_3", "Pass 3 - Writing random data", 2, 3}
    };
    return stages;
}

void NAVSOP523926MFM::execute(Disk& disk, DiskDeleteMethod::Delegate& delegate) {
    const auto& stages = getStages();

    static constexpr unsigned char pattern1[] = {0x01};
    static constexpr unsigned char pattern2[] = {0x27, 0xFF, 0xFF, 0xFF};

    delegate.onStageStarted(stages[0]);
    write(disk, pattern1, [&](const Progress& progress) {
        delegate.onProgress(stages[0], progress);
    });
    delegate.onStageCompleted(stages[0]);

    if (delegate.shouldCancel()) return;

    delegate.onStageStarted(stages[1]);
    write(disk, pattern2, [&](const Progress& progress) {
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

void NAVSOP523926MFM::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void NAVSOP523926MFM::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }
void NAVSOP523926MFM::deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) { execute(disk, delegate); }

} // namespace DiskManagement

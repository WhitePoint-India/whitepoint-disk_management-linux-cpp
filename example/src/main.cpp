
#include <memory>
#include <iostream>
#include <disk-management>

class ATAClearDelegate : public NIST_800_88_Clear::ATA::Delegate {
public:
    void didChangeStage(NIST_800_88_Clear::ATA::Stage stage) override {
        switch (stage) {
        case NIST_800_88_Clear::ATA::Stage::DISK_FREEZE_CHECK:
            std::cout << "[NIST Clear] Checking disk freeze state..." << std::endl;
            break;
        case NIST_800_88_Clear::ATA::Stage::ERASURE:
            std::cout << "[NIST Clear] Starting erasure..." << std::endl;
            break;
        case NIST_800_88_Clear::ATA::Stage::VERIFICATION:
            std::cout << "[NIST Clear] Starting verification..." << std::endl;
            break;
        default:
            break;
        }
    }

    void didChangeProgress(NIST_800_88_Clear::ATA::Stage stage, const Progress& progress) override {
        switch (stage) {
        case NIST_800_88_Clear::ATA::Stage::ERASURE:
            std::cout << "\r[NIST Clear] Erasure: " << progress.percentageCompleted() << "%" << std::flush;
            break;
        case NIST_800_88_Clear::ATA::Stage::VERIFICATION:
            std::cout << "\r[NIST Clear] Verification: " << progress.percentageCompleted() << "%" << std::flush;
            break;
        default:
            break;
        }
    }

    void didCompleteSuccessfully(const NIST_800_88_Clear::ATA::Result& result) override {
        std::cout << std::endl;
        std::cout << "[NIST Clear] Sanitization complete." << std::endl;
        std::cout << "  Started:    " << result.startedAt << std::endl;
        std::cout << "  Completed:  " << result.completedAt << std::endl;
        std::cout << "  Duration:   " << result.durationSeconds << "s" << std::endl;
        std::cout << "  Verified:   " << result.verification.sectorsVerified
                  << "/" << result.verification.sectorsTotal << " sectors" << std::endl;
        std::cout << "  Non-zero:   " << result.verification.nonZeroSectors << std::endl;
        std::cout << "  Passed:     " << (result.verification.passed ? "YES" : "NO") << std::endl;
    }

    void didCompleteWithError(const NIST_800_88_Clear::ATA::Error& error) override {
        std::cout << std::endl;
        std::cerr << "[NIST Clear] ERROR: " << error.message << std::endl;
    }
};

int main() {
    std::vector<std::unique_ptr<DiskManagement::Disk>> disks = DiskManagement::fetchDisks();
    for (const std::unique_ptr<DiskManagement::Disk>& diskPtr : disks) {
        if (auto ataDisk = dynamic_cast<DiskManagement::ATADisk*>(diskPtr.get())) {
            ATAClearDelegate delegate;
            ataDisk->deleteDisk(DiskManagement::NIST_800_88_CLEAR, delegate);
        }
        else if (auto usbDisk = dynamic_cast<DiskManagement::USBDisk*>(diskPtr.get())) {
            // USB sanitization not supported — use physical destruction
        }
    }
    return 0;
}

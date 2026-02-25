
#include <nist_800_88_clear.hpp>

namespace DiskManagement {

NIST80088Clear::NIST80088Clear() : SingletonMethod("NIST_800_88_CLEAR") {

}

const std::string& NIST80088Clear::getTitle() const {
    static const std::string title = "NIST 800-88 CLEAR";
    return title;
}

const std::string& NIST80088Clear::getDescription() const {
    static const std::string description = "NIST 800-88 Clear level sanitization. Applies logical techniques to sanitize data in all user-addressable storage locations.";
    return description;
}

const std::vector<Stage>& NIST80088Clear::getStages() const {
    static const std::vector<Stage> stages = {
        {"zero_overwrite", "Zero overwrite", 0, 2},
        {"verification", "Verification", 1, 2}
    };
    return stages;
}

void NIST80088Clear::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) {
    auto stages = getStages();

    // Stage 1: Zero overwrite via dd
    delegate.onStageStarted(stages[0]);

    unsigned long long totalBytes = disk.getSize();
    unsigned long long bytesWritten = 0;
    unsigned long long chunkSize = 1024 * 1024; // 1MB

    while (bytesWritten < totalBytes) {
        if (delegate.shouldCancel()) { return; }

        unsigned long long remaining = totalBytes - bytesWritten;
        unsigned long long writeSize = remaining < chunkSize ? remaining : chunkSize;

        // TODO: actual dd if=/dev/zero of=[disk] bs=1M oflag=direct
        bytesWritten += writeSize;

        delegate.onProgress(stages[0], {bytesWritten, totalBytes});
    }

    delegate.onStageCompleted(stages[0]);

    // Stage 2: Verification
    delegate.onStageStarted(stages[1]);

    unsigned long long bytesVerified = 0;

    while (bytesVerified < totalBytes) {
        if (delegate.shouldCancel()) { return; }

        unsigned long long remaining = totalBytes - bytesVerified;
        unsigned long long readSize = remaining < chunkSize ? remaining : chunkSize;

        // TODO: actual verification read
        bytesVerified += readSize;

        delegate.onProgress(stages[1], {bytesVerified, totalBytes});
    }

    delegate.onStageCompleted(stages[1]);
    delegate.onCompleted();
}

void NIST80088Clear::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) {
    auto stages = getStages();

    // Stage 1: NVMe format command
    delegate.onStageStarted(stages[0]);

    // TODO: actual nvme format [disk] -s 1 -f
    delegate.onProgress(stages[0], {disk.getSize(), disk.getSize()});

    delegate.onStageCompleted(stages[0]);

    // Stage 2: Verification
    delegate.onStageStarted(stages[1]);

    unsigned long long totalBytes = disk.getSize();
    unsigned long long bytesVerified = 0;
    unsigned long long chunkSize = 1024 * 1024;

    while (bytesVerified < totalBytes) {
        if (delegate.shouldCancel()) { return; }

        unsigned long long remaining = totalBytes - bytesVerified;
        unsigned long long readSize = remaining < chunkSize ? remaining : chunkSize;

        // TODO: actual verification read
        bytesVerified += readSize;

        delegate.onProgress(stages[1], {bytesVerified, totalBytes});
    }

    delegate.onStageCompleted(stages[1]);
    delegate.onCompleted();
}

} // namespace DiskManagement

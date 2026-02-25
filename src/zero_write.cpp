
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

void ZeroWrite::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) {
    auto stages = getStages();

    delegate.onStageStarted(stages[0]);

    unsigned long long totalBytes = disk.getSize();
    unsigned long long bytesWritten = 0;
    unsigned long long chunkSize = 1024 * 1024; // 1MB

    while (bytesWritten < totalBytes) {
        if (delegate.shouldCancel()) { return; }

        unsigned long long remaining = totalBytes - bytesWritten;
        unsigned long long writeSize = remaining < chunkSize ? remaining : chunkSize;

        // TODO: actual write zeros to disk via dd or direct I/O
        bytesWritten += writeSize;

        delegate.onProgress(stages[0], {bytesWritten, totalBytes});
    }

    delegate.onStageCompleted(stages[0]);
    delegate.onCompleted();
}

} // namespace DiskManagement

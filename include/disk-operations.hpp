#ifndef DISK_OPERATIONS_H
#define DISK_OPERATIONS_H

#include <disk.hpp>
#include <functional>

class Progress {
private:
    unsigned long long totalBytes;
    unsigned long long bytesProcessed;
public:

    Progress(
        unsigned long long bytesProcessed,
        unsigned long long totalBytes
    ) : totalBytes(totalBytes), bytesProcessed(bytesProcessed) {
    }

    double fractionCompleted() const {
        if (totalBytes == 0) {
            return 0;
        }
        return static_cast<double>(bytesProcessed) / static_cast<double>(totalBytes);
    }

    double percentageCompleted() const {
        return fractionCompleted() * 100;
    }
};

template <typename Disk, typename Delegate>
class AnyDeleteOperation {
public:
    virtual ~AnyDeleteOperation() = default;
    virtual void deleteDisk(Disk& disk, Delegate& delegate) const = 0;
};

class Writable {

public:
    enum class Method {
        xFF,
        x0,
        RANDOM
    };

    using Callback = std::function<void(const Progress&)>;

    void write(DiskManagement::Disk& disk, Method method, Callback callback) const;

private:
    static void randomize(void* buffer, int urandom);
    static void writeToDisk(DiskManagement::Disk& disk, int diskFD, void* buffer, Callback callback, std::function<void(void*, size_t)> refill);
    static void writeBytes(DiskManagement::Disk& disk, void* buffer, Callback callback);
    static void writeRandomBytes(DiskManagement::Disk& disk, void* buffer, Callback callback);
};

#endif // DISK_OPERATIONS_H
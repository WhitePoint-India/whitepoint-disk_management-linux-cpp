
#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP

#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <disks.hpp>
#include <functional>

namespace DiskManagement {

struct Stage {
    std::string_view id;
    std::string_view name;
    int index;
    int total;
};

class Progress {
private:
    const unsigned long long _bytesProcessed;
    const unsigned long long _totalBytes;
public:
    Progress(unsigned long long bytesProcessed, unsigned long long totalBytes);
    [[nodiscard]] unsigned long long bytesProcessed() const;
    [[nodiscard]] unsigned long long totalBytes() const;
    [[nodiscard]] double fractionCompleted() const;
    [[nodiscard]] double percentageCompleted() const;
};

class Writable {

public:
    enum class Method {
        xFF,
        x0,
        RANDOM
    };

    using Callback = std::function<void(const Progress&)>;

    void write(Disk& disk, Method method, Callback callback) const;
    void write(Disk& disk, std::span<const unsigned char> pattern, Callback callback) const;

private:
    static void fillPattern(void* buffer, size_t bufferSize, std::span<const unsigned char> pattern);
    static void randomize(void* buffer, int urandom);
    static void writeToDisk(Disk& disk, int diskFD, void* buffer, Callback callback, std::function<void(void*, size_t)> refill);
    static void writeBytes(Disk& disk, void* buffer, Callback callback);
    static void writeRandomBytes(Disk& disk, void* buffer, Callback callback);
};

struct VerificationResult {
    unsigned long long sectorsVerified;
    unsigned long long sectorsTotal;
    unsigned long long nonZeroSectors;
    double samplingPercentage;
    bool passed;
};

class Verifiable {
public:
    using Callback = std::function<void(const Progress&)>;

    VerificationResult verifySampling(const Disk& disk, double samplingPercentage, double passThreshold, Callback callback) const;
    VerificationResult verifyFull(const Disk& disk, Callback callback) const;
};

class DiskDeleteMethod {
private:
    const std::string id;
protected:
    DiskDeleteMethod(const std::string& id);
public:
    class Delegate {
        public:
        virtual void onStageStarted(const Stage& stage) = 0;
        virtual void onProgress(const Stage& stage, const Progress& progress) = 0;
        virtual void onStageCompleted(const Stage& stage) = 0;
        virtual void onCompleted() = 0;
        virtual void onError(const std::string& message) = 0;
        [[nodiscard]] virtual bool shouldCancel();
        virtual ~Delegate() = default;
    };

    [[nodiscard]] const std::string& getID() const;
    [[nodiscard]] virtual const std::string& getTitle() const = 0;
    [[nodiscard]] virtual const std::string& getDescription() const = 0;

    [[nodiscard]] virtual const std::vector<Stage>& getStages() const = 0;

    virtual void deleteDisk(Disks::ATADisk& disk, Delegate& delegate);
    virtual void deleteDisk(Disks::NVMeDisk& disk, Delegate& delegate);
    virtual void deleteDisk(Disks::USBDisk& disk, Delegate& delegate);

    virtual ~DiskDeleteMethod() = default;
};

template<typename Derived>
class SingletonMethod : public DiskDeleteMethod {
protected:
    using DiskDeleteMethod::DiskDeleteMethod;
public:
    SingletonMethod(const SingletonMethod&) = delete;
    SingletonMethod& operator=(const SingletonMethod&) = delete;
    SingletonMethod(SingletonMethod&&) = delete;
    SingletonMethod& operator=(SingletonMethod&&) = delete;

    [[nodiscard]] static Derived& shared() {
        static Derived instance;
        return instance;
    }
};

} // namespace DiskManagement

#endif // OPERATIONS_HPP
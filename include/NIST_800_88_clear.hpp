
#include <string>
#include <disks.hpp>

namespace NIST_800_88_Clear {

namespace ATA {

enum class Stage {
    DISK_FREEZE_CHECK,
    ERASURE,
    VERIFICATION,
    COUNT
};

struct Result {
    std::string startedAt;
    std::string completedAt;
    unsigned long long durationSeconds;

    struct Verification {
        unsigned long long sectorsVerified;
        unsigned long long sectorsTotal;
        unsigned long long nonZeroSectors;
        double samplingPercentage;
        double passThreshold;
        bool passed;
    } verification;
};

struct Error {
    enum class Code {
        DISK_FROZEN,
        PERMISSION_DENIED,
        WRITE_FAILED,
        VERIFICATION_FAILED
    };
    Code code;
    std::string message;
};

class Delegate {
public:
    virtual ~Delegate() = default;
    virtual void didChangeStage(Stage stage) = 0;
    virtual void didChangeProgress(Stage stage, const Progress& progress) = 0;
    virtual void didCompleteSuccessfully(const Result& result) = 0;
    virtual void didCompleteWithError(const Error& error) = 0;
};

} // namespace ATA

class DeleteOperation : public DiskManagement::ATADiskDeleteOperation<ATA::Delegate>, private Writable {
public:
    void deleteDisk(DiskManagement::ATADisk& disk, ATA::Delegate& delegate) const override;
};

} // namespace NIST_800_88_Clear


#include <NIST_800_88_clear.hpp>
#include <disk-verifier.hpp>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {

std::string nowISO8601() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm utc{};
    gmtime_r(&time, &utc);
    std::ostringstream ss;
    ss << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

} // anonymous namespace

namespace NIST_800_88_Clear {

void DeleteOperation::deleteDisk(DiskManagement::ATADisk& disk, ATA::Delegate& delegate) const {

    std::string startedAt = nowISO8601();
    auto startTime = std::chrono::steady_clock::now();

    // Stage 1: Freeze check
    delegate.didChangeStage(ATA::Stage::DISK_FREEZE_CHECK);

    try {
        if (disk.isFrozen()) {
            delegate.didCompleteWithError(ATA::Error{
                .code = ATA::Error::Code::DISK_FROZEN,
                .message = "Disk " + disk.path + " is in a frozen security state. "
                           "A system suspend/resume cycle is required to unfreeze."
            });
            return;
        }
    } catch (const std::exception& e) {
        delegate.didCompleteWithError(ATA::Error{
            .code = ATA::Error::Code::PERMISSION_DENIED,
            .message = "Failed to check frozen state of " + disk.path + ": " + e.what()
        });
        return;
    }

    // Stage 2: Erasure (single-pass zero overwrite via Direct I/O)
    delegate.didChangeStage(ATA::Stage::ERASURE);

    try {
        write(disk, Writable::Method::x0, [&](const Progress& progress) {
            delegate.didChangeProgress(ATA::Stage::ERASURE, progress);
        });
    } catch (const std::exception& e) {
        delegate.didCompleteWithError(ATA::Error{
            .code = ATA::Error::Code::WRITE_FAILED,
            .message = "Erasure failed on " + disk.path + ": " + e.what()
        });
        return;
    }

    // Stage 3: Verification (full sequential read, zero non-zero sectors tolerance)
    // NIST 800-88 Rev. 1: "the highest level of assurance ... is typically achieved
    // by a full reading of all accessible areas to verify that the expected sanitized
    // value is in all addressable locations."
    delegate.didChangeStage(ATA::Stage::VERIFICATION);

    VerificationResult verification;
    try {
        verification = DiskVerifier::verifyFull(
            disk,
            [&](const Progress& progress) {
                delegate.didChangeProgress(ATA::Stage::VERIFICATION, progress);
            }
        );
    } catch (const std::exception& e) {
        delegate.didCompleteWithError(ATA::Error{
            .code = ATA::Error::Code::VERIFICATION_FAILED,
            .message = "Verification error on " + disk.path + ": " + e.what()
        });
        return;
    }

    if (!verification.passed) {
        delegate.didCompleteWithError(ATA::Error{
            .code = ATA::Error::Code::VERIFICATION_FAILED,
            .message = "Verification failed: " + std::to_string(verification.nonZeroSectors)
                       + " non-zero sectors found out of " + std::to_string(verification.sectorsVerified)
                       + " verified"
        });
        return;
    }

    // Success
    auto endTime = std::chrono::steady_clock::now();
    std::string completedAt = nowISO8601();
    auto durationSeconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

    delegate.didCompleteSuccessfully(ATA::Result{
        .startedAt = startedAt,
        .completedAt = completedAt,
        .durationSeconds = static_cast<unsigned long long>(durationSeconds),
        .verification = {
            .sectorsVerified = verification.sectorsVerified,
            .sectorsTotal = verification.sectorsTotal,
            .nonZeroSectors = verification.nonZeroSectors,
            .samplingPercentage = 1.0,
            .passThreshold = 0.0,
            .passed = verification.passed
        }
    });
}

} // namespace NIST_800_88_Clear

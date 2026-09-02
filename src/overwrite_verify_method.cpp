
#include <overwrite_verify_method.hpp>

#include <span>
#include <stdexcept>
#include <string>

#include <disk.hpp>
#include <block_writable.hpp>
#include <verifiable.hpp>
#include <sanitization_progress.hpp>

void OverwriteVerifyMethod::sanitize(Disk& disk, DiskSanitizationInterface::Callback callback) {
    const int total = passCount() + 1;

    // Check both capabilities up front so an unsupported disk fails before any
    // destructive writes, not after hours of overwriting.
    auto* writable = dynamic_cast<BlockWritable*>(&disk);
    if (!writable) {
        throw std::invalid_argument(getKey() + " requires a block-writable disk");
    }
    auto* verifiable = dynamic_cast<Verifiable*>(&disk);
    if (!verifiable) {
        throw std::invalid_argument(getKey() + " requires a verifiable disk");
    }

    // Stages 0..N-1: overwrite passes. SanitizationProgress takes a non-const
    // stage reference (it never mutates it); the engine hands us a const Pass,
    // so cast away constness here.
    write(*writable, [&](const Pass& pass, int index, int /*passes*/, double fraction) {
        callback(SanitizationProgress(const_cast<Pass&>(pass), index, total, fraction));
    });

    // Stage N: read back 100% of the disk; every sector must be all 0x00,
    // matching the final zero pass.
    static constexpr unsigned char kZero[] = { 0x00 };
    const Verifiable::VerificationResult result = verifiable->verify(
        std::span<const unsigned char>(kZero),
        [&](double fraction) {
            callback(SanitizationProgress(verifyStage_, passCount(), total, fraction));
        });

    if (!result.passed) {
        throw std::runtime_error(getKey() + ": verification failed: "
            + std::to_string(result.mismatches) + " of "
            + std::to_string(result.sectorsChecked)
            + " sectors did not read back as zeros");
    }
}

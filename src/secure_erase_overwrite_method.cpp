
#include <secure_erase_overwrite_method.hpp>

#include <stdexcept>

#include <disk.hpp>
#include <secure_erase.hpp>
#include <block_writable.hpp>
#include <sanitization_progress.hpp>

void SecureEraseOverwriteMethod::sanitize(Disk& disk, DiskSanitizationInterface::Callback callback) {
    const int total = 1 + passCount();

    // Stage 0: secure erase. Reuse the SecureErase method (ATA secure erase / NVMe
    // sanitize block-erase) and re-wrap its progress into this method's numbering.
    // SecureErase reports (stage, 0, 1, partial), so p.fractionCompleted() == partial.
    SecureErase::shared().sanitize(disk, [&](const SanitizationProgress p) {
        callback(SanitizationProgress(
            const_cast<LocalizableSanitizationStage&>(p.getStage()), 0, total, p.fractionCompleted()));
    });

    // Stages 1..N: overwrite passes.
    auto* writable = dynamic_cast<BlockWritable*>(&disk);
    if (!writable) {
        throw std::invalid_argument(getKey() + " requires a block-writable disk");
    }
    write(*writable, [&](const Pass& pass, int index, int /*passes*/, double fraction) {
        callback(SanitizationProgress(const_cast<Pass&>(pass), index + 1, total, fraction));
    });
}

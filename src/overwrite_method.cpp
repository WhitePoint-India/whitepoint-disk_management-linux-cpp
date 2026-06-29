
#include <overwrite_method.hpp>

#include <stdexcept>

#include <disk.hpp>
#include <block_writable.hpp>
#include <sanitization_progress.hpp>

void OverwriteMethod::sanitize(Disk& disk, DiskSanitizationInterface::Callback callback) {
    auto* writable = dynamic_cast<BlockWritable*>(&disk);
    if (!writable) {
        throw std::invalid_argument(getKey() + " requires a block-writable disk");
    }

    write(*writable, [&](const Pass& pass, int index, int total, double fraction) {
        // SanitizationProgress takes a non-const stage reference (it never mutates
        // it); the engine hands us a const Pass, so cast away constness here.
        callback(SanitizationProgress(const_cast<Pass&>(pass), index, total, fraction));
    });
}

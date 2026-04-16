
#include <nist_purge.hpp>
#include <block_writable.hpp>

#include <stdexcept>

NISTPurge::NISTPurge(): DiskSanitizationInterface("NIST_800_88_PURGE"), AutoRegisterMethod(*this) {}

NISTPurge& NISTPurge::shared() {
    static NISTPurge instance;
    return instance;
}

void NISTPurge::sanitize(Disk& disk, Callback callback) {
    auto* writable = dynamic_cast<BlockWritable*>(&disk);
    if (!writable) {
        throw std::invalid_argument("NIST Purge requires a block-writable disk");
    }
    // TODO: Implement NIST 800-88 Purge sanitization
}

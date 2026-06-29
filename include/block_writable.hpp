
#ifndef BLOCK_WRITABLE_HPP
#define BLOCK_WRITABLE_HPP

#include <cstddef>
#include <cstdint>

// Minimal interface through which data is written to a disk. The disk-wide
// overwrite loop lives in BlockWritingMethod::Pass, which drives these
// primitives; concrete disks supply only the four operations below.
class BlockWritable {
public:
    virtual ~BlockWritable() noexcept = default;

    // Primitives implemented by concrete disks.
    virtual void writeBlock(uint64_t sectorOffset, const void* data, std::size_t dataSize) = 0;
    virtual void flush() = 0;  // durably commit writes (fdatasync)

    [[nodiscard]] virtual unsigned long long getSectorCount() const noexcept = 0;
    [[nodiscard]] virtual unsigned int getSectorSize() const noexcept = 0;
};

#endif // BLOCK_WRITABLE_HPP

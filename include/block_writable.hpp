
#ifndef BLOCK_WRITABLE_HPP
#define BLOCK_WRITABLE_HPP

#include <cstddef>
#include <cstdint>
#include <span>
#include <functional>

class BlockWritable {
public:
    // Fill patterns for a full-disk overwrite pass.
    enum class Fill { Zero, Ones, Random };

    // Reports the completion fraction in [0.0, 1.0] as a pass proceeds.
    using ProgressCallback = std::function<void(double fractionCompleted)>;

    virtual ~BlockWritable() noexcept = default;

    // Primitives implemented by concrete disks.
    virtual void writeBlock(uint64_t sectorOffset, const void* data, std::size_t dataSize) = 0;
    virtual void flush() = 0;  // durably commit writes (fdatasync)

    [[nodiscard]] virtual unsigned long long getSectorCount() const noexcept = 0;
    [[nodiscard]] virtual unsigned int getSectorSize() const noexcept = 0;

    // Overwrite the entire disk once with `fill` (or a repeating `pattern`),
    // reporting progress, then flush(). Implemented in terms of the primitives
    // above, so it is shared by every disk type and every delete method.
    void overwrite(Fill fill, const ProgressCallback& onProgress);
    void overwrite(std::span<const unsigned char> pattern, const ProgressCallback& onProgress);
};

#endif // BLOCK_WRITABLE_HPP

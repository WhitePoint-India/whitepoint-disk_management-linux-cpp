
#ifndef BLOCK_WRITABLE_HPP
#define BLOCK_WRITABLE_HPP

#include <cstddef>
#include <cstdint>

class BlockWritable {
public:
    virtual ~BlockWritable() noexcept = default;

    virtual void writeBlock(uint64_t sectorOffset, const void* data, std::size_t dataSize) = 0;

    [[nodiscard]] virtual unsigned long long getSectorCount() const noexcept = 0;
    [[nodiscard]] virtual unsigned int getSectorSize() const noexcept = 0;
};

#endif // BLOCK_WRITABLE_HPP

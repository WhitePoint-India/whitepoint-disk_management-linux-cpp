
#ifndef NVME_DISK_HPP
#define NVME_DISK_HPP

#include <disk.hpp>
#include <block_writable.hpp>
#include <nvme_sanitizable.hpp>

class NVMeDisk: public Disk, public BlockWritable, public NVMeSanitizable {
public:
    using Disk::Disk;

    void writeBlock(uint64_t sectorOffset, const void* data, std::size_t dataSize) override;
    void sanitize(Action action, Callback callback) override;
    void formatNVM(Callback callback) override;

    [[nodiscard]] unsigned long long getSectorCount() const noexcept override { return Disk::getSectorCount(); }
    [[nodiscard]] unsigned int getSectorSize() const noexcept override { return Disk::getSectorSize(); }
};

#endif // NVME_DISK_HPP

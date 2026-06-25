
#ifndef NVME_DISK_HPP
#define NVME_DISK_HPP

#include <disk.hpp>
#include <block_writable.hpp>
#include <verifiable.hpp>
#include <nvme_sanitizable.hpp>

class NVMeDisk: public Disk, public BlockWritable, public Verifiable, public NVMeSanitizable {
private:
    int blockFd_ = -1;
    int blockFd();  // lazily-opened cached O_RDWR | O_DIRECT descriptor

public:
    using Disk::Disk;
    ~NVMeDisk() noexcept override;

    void writeBlock(uint64_t sectorOffset, const void* data, std::size_t dataSize) override;
    void readBlock(uint64_t sectorOffset, void* data, std::size_t dataSize) override;
    void flush() override;
    void sanitize(Action action, Callback callback) override;
    void formatNVM(Callback callback) override;

    [[nodiscard]] unsigned long long getSectorCount() const noexcept override { return Disk::getSectorCount(); }
    [[nodiscard]] unsigned int getSectorSize() const noexcept override { return Disk::getSectorSize(); }
};

#endif // NVME_DISK_HPP

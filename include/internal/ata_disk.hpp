
#ifndef ATA_DISK_HPP
#define ATA_DISK_HPP

#include <disk.hpp>
#include <block_writable.hpp>
#include <verifiable.hpp>
#include <ata_secure_erasable.hpp>

class ATADisk: public Disk, public BlockWritable, public Verifiable, public ATASecureErasable {

private:
    bool supportsEnhancedErase() const override;

    int blockFd_ = -1;
    int blockFd();  // lazily-opened cached O_RDWR | O_DIRECT descriptor

public:
    using Disk::Disk;
    ~ATADisk() noexcept override;

    [[nodiscard]] bool isFrozen() const override;
    void unfreeze() override;
    void secureErase(Callback callback) override;
    void secureEraseEnhanced(Callback callback) override;

    void writeBlock(uint64_t sectorOffset, const void* data, std::size_t dataSize) override;
    void readBlock(uint64_t sectorOffset, void* data, std::size_t dataSize) override;
    void flush() override;

    [[nodiscard]] unsigned long long getSectorCount() const noexcept override { return Disk::getSectorCount(); }
    [[nodiscard]] unsigned int getSectorSize() const noexcept override { return Disk::getSectorSize(); }
};

#endif // ATA_DISK_HPP


#ifndef ATA_DISK_HPP
#define ATA_DISK_HPP

#include <disk.hpp>
#include <block_writable.hpp>
#include <ata_secure_erasable.hpp>

class ATADisk: public Disk, public BlockWritable, public ATASecureErasable {
public:
    using Disk::Disk;

    [[nodiscard]] bool isFrozen() const override;
    void unfreeze() override;
    void secureEraseUnit(bool enhanced) override;
    void writeBlock(uint64_t sectorOffset, const void* data, std::size_t dataSize) override;

    [[nodiscard]] unsigned long long getSectorCount() const noexcept override { return Disk::getSectorCount(); }
    [[nodiscard]] unsigned int getSectorSize() const noexcept override { return Disk::getSectorSize(); }
};

#endif // ATA_DISK_HPP

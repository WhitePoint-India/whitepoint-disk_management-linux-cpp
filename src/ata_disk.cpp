
#include <stdexcept>
#include <ata_disk.hpp>

#include <array>
#include <fcntl.h>
#include <cstdint>
#include <unistd.h>
#include <scsi/sg.h>
#include <sys/ioctl.h>

#include <iostream>

namespace {

// ATA IDENTIFY DEVICE returns 256 little-endian 16-bit words (512 bytes).
constexpr std::size_t kIdentifyWordCount = 256;

// Issue ATA IDENTIFY DEVICE (0xEC) via the SCSI ATA PASS-THROUGH (16) command
// over the SG_IO ioctl. This is the libata-compatible path for SATA/PATA
// drives presented as /dev/sdX. Returns true and fills `words` on success.
bool readIdentifyDevice(const std::string& path,
                        std::array<uint16_t, kIdentifyWordCount>& words) {
    const int fd = ::open(path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        return false;
    }

    std::array<uint8_t, 512> data{};
    std::array<uint8_t, 32> sense{};

    // ATA PASS-THROUGH (16) CDB for a 28-bit PIO data-in command.
    uint8_t cdb[16] = {};
    cdb[0]  = 0x85;  // ATA PASS-THROUGH (16)
    cdb[1]  = 0x08;  // PROTOCOL = PIO Data-In (4 << 1), EXTEND = 0
    cdb[2]  = 0x0E;  // T_DIR = from device, BYT_BLOK = 1, T_LENGTH = SECTOR_COUNT
    cdb[6]  = 0x01;  // SECTOR_COUNT = 1 (read one 512-byte block)
    cdb[14] = 0xEC;  // IDENTIFY DEVICE

    sg_io_hdr_t io{};
    io.interface_id    = 'S';
    io.dxfer_direction = SG_DXFER_FROM_DEV;
    io.cmd_len         = sizeof(cdb);
    io.mx_sb_len       = static_cast<unsigned char>(sense.size());
    io.dxfer_len       = static_cast<unsigned int>(data.size());
    io.dxferp          = data.data();
    io.cmdp            = cdb;
    io.sbp             = sense.data();
    io.timeout         = 5000;  // milliseconds

    const int rc = ::ioctl(fd, SG_IO, &io);
    ::close(fd);

    if (rc < 0 || (io.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
        return false;
    }

    // Reassemble each word from its little-endian byte pair explicitly, so the
    // result is correct regardless of host byte order.
    for (std::size_t i = 0; i < kIdentifyWordCount; ++i) {
        words[i] = static_cast<uint16_t>(data[2 * i] |
                                         (static_cast<uint16_t>(data[2 * i + 1]) << 8));
    }
    return true;
}

}  // namespace

bool ATADisk::isFrozen() const {

    return true;
}

void ATADisk::unfreeze() {

}

void ATADisk::writeBlock(uint64_t /*sectorOffset*/, const void* /*data*/, std::size_t /*dataSize*/) {

}

void ATADisk::secureErase() {
    /// @todo Implement secure erase using ioctl
    std::cout << "SecureErase called" << std::endl;
}

void ATADisk::secureEraseEnhanced() {
    if (supportsEnhancedErase()) {
        /// @todo Implement enhanced secure erase using ioctl
    std::cout << "Enhanced SecureErase called" << std::endl;
    } else {
        throw std::runtime_error("Enhanced Secure Erase is not supported for this disk");
    }
}

bool ATADisk::supportsEnhancedErase() const {
    // ATA IDENTIFY DEVICE word 128 (Security status), bit 5 = "Enhanced
    // security erase supported". If IDENTIFY cannot be read, conservatively
    // report no support rather than claim a capability we could not verify.
    std::array<uint16_t, kIdentifyWordCount> words{};
    if (!readIdentifyDevice(getPath(), words)) {
        return false;
    }

    constexpr std::size_t kSecurityStatusWord = 128;
    constexpr uint16_t kEnhancedEraseSupportedBit = 1u << 5;
    return (words[kSecurityStatusWord] & kEnhancedEraseSupportedBit) != 0;
}


#include <stdexcept>
#include <string>
#include <array>
#include <cstdint>
#include <cstring>
#include <cerrno>
#include <system_error>

#include <fcntl.h>
#include <unistd.h>
#include <scsi/sg.h>
#include <sys/ioctl.h>

#include <ata_disk.hpp>
#include <block_io.hpp>

namespace {

// ATA IDENTIFY DEVICE returns 256 little-endian 16-bit words (512 bytes).
constexpr std::size_t kIdentifyWordCount = 256;

// IDENTIFY DEVICE word indices of interest.
constexpr std::size_t kSecurityStatusWord = 128;  // word 128: Security status
constexpr std::size_t kNormalEraseTimeWord = 89;   // word 89:  normal erase time
constexpr std::size_t kEnhancedEraseTimeWord = 90; // word 90:  enhanced erase time

// IDENTIFY DEVICE word 128 (Security status) bit masks.
constexpr uint16_t kSecSupported       = 1u << 0;
constexpr uint16_t kSecEnabled         = 1u << 1;
constexpr uint16_t kSecLocked          = 1u << 2;
constexpr uint16_t kSecFrozen          = 1u << 3;
constexpr uint16_t kSecEnhancedSupport = 1u << 5;

// ATA Security feature set command opcodes.
constexpr uint8_t kAtaIdentifyDevice      = 0xEC;
constexpr uint8_t kAtaSecuritySetPassword = 0xF1;
constexpr uint8_t kAtaSecurityErasePrepare = 0xF3;
constexpr uint8_t kAtaSecurityEraseUnit   = 0xF4;
constexpr uint8_t kAtaSecurityDisable     = 0xF6;

// Ephemeral password used to enable security so ERASE UNIT can run; the erase
// clears it on success. A *known* constant (not random) is used deliberately:
// if the process dies between SET PASSWORD and ERASE UNIT, the drive is left
// security-enabled and can be recovered with
//   hdparm --user-master u --security-disable JungleJapan /dev/sdX
constexpr char kTempPassword[] = "JungleJapan";

constexpr unsigned kShortTimeoutMs = 15000;  // for non-erase commands

enum class AtaDir { None, FromDevice, ToDevice };

// Issue a single ATA command via the SCSI ATA PASS-THROUGH (16) CDB over SG_IO.
// This is the libata-compatible path for SATA/PATA drives presented as /dev/sdX.
// `buf` is a 512-byte block for FromDevice/ToDevice transfers (ignored for None).
// Returns true only when the command completed without any SCSI/host/driver error.
bool issueAtaCommand(int fd, uint8_t ataOp, AtaDir dir, uint8_t* buf,
                     unsigned countSectors, unsigned timeoutMs) {
    uint8_t protocol = 3;             // 3 = Non-data
    uint8_t flags    = 0x00;          // cdb[2]: T_DIR / BYT_BLOK / T_LENGTH
    int sgDir        = SG_DXFER_NONE;
    unsigned xferLen = 0;

    switch (dir) {
        case AtaDir::None:
            protocol = 3;             // Non-data
            flags    = 0x00;          // T_LENGTH = 0 (no data)
            sgDir    = SG_DXFER_NONE;
            break;
        case AtaDir::FromDevice:
            protocol = 4;             // PIO Data-In
            flags    = 0x0E;          // T_DIR = from device, BYT_BLOK = 1, T_LENGTH = SECTOR_COUNT
            sgDir    = SG_DXFER_FROM_DEV;
            xferLen  = countSectors * 512u;
            break;
        case AtaDir::ToDevice:
            protocol = 5;             // PIO Data-Out
            flags    = 0x06;          // T_DIR = to device, BYT_BLOK = 1, T_LENGTH = SECTOR_COUNT
            sgDir    = SG_DXFER_TO_DEV;
            xferLen  = countSectors * 512u;
            break;
    }

    std::array<uint8_t, 32> sense{};
    uint8_t cdb[16] = {};
    cdb[0]  = 0x85;                                       // ATA PASS-THROUGH (16)
    cdb[1]  = static_cast<uint8_t>(protocol << 1);        // PROTOCOL, EXTEND = 0
    cdb[2]  = flags;
    cdb[6]  = static_cast<uint8_t>(countSectors);         // SECTOR_COUNT (7:0)
    cdb[14] = ataOp;                                      // COMMAND

    sg_io_hdr_t io{};
    io.interface_id    = 'S';
    io.dxfer_direction = sgDir;
    io.cmd_len         = sizeof(cdb);
    io.mx_sb_len       = static_cast<unsigned char>(sense.size());
    io.dxfer_len       = xferLen;
    io.dxferp          = (dir == AtaDir::None) ? nullptr : buf;
    io.cmdp            = cdb;
    io.sbp             = sense.data();
    io.timeout         = timeoutMs;

    if (::ioctl(fd, SG_IO, &io) < 0) {
        return false;
    }
    return (io.info & SG_INFO_OK_MASK) == SG_INFO_OK;
}

// Read ATA IDENTIFY DEVICE. Returns the 256 words; throws std::system_error
// (with errno) if the device cannot be opened, or std::runtime_error if the
// IDENTIFY command itself fails.
std::array<uint16_t, kIdentifyWordCount> readIdentifyData(const std::string& path) {
    const int fd = ::open(path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        throw std::system_error(errno, std::generic_category(), "open " + path);
    }

    std::array<uint8_t, 512> data{};
    const bool ok = issueAtaCommand(fd, kAtaIdentifyDevice, AtaDir::FromDevice,
                                    data.data(), 1, kShortTimeoutMs);
    ::close(fd);
    if (!ok) {
        throw std::runtime_error("ATA IDENTIFY DEVICE command failed on " + path);
    }

    // Reassemble each word from its little-endian byte pair explicitly, so the
    // result is correct regardless of host byte order.
    std::array<uint16_t, kIdentifyWordCount> words {};
    for (std::size_t i = 0; i < kIdentifyWordCount; ++i) {
        words[i] = static_cast<uint16_t>(data[2 * i] |
                                         (static_cast<uint16_t>(data[2 * i + 1]) << 8));
    }
    return words;
}

// Total addressable sectors, from the LBA48 (words 100-103) or LBA28 (60-61) fields.
uint64_t lbaSectors(const std::array<uint16_t, kIdentifyWordCount>& id) {
    const uint64_t lba48 = static_cast<uint64_t>(id[100]) |
                           (static_cast<uint64_t>(id[101]) << 16) |
                           (static_cast<uint64_t>(id[102]) << 32) |
                           (static_cast<uint64_t>(id[103]) << 48);
    if (lba48 != 0) {
        return lba48;
    }
    return static_cast<uint64_t>(id[60]) | (static_cast<uint64_t>(id[61]) << 16);
}

// Erase timeout in seconds, mirroring hdparm's get_erase_timeout_secs():
// decode IDENTIFY word 89 (normal) / 90 (enhanced); fall back to a
// capacity-derived estimate (1s per 30MB + 30min), capped, when unspecified.
uint64_t eraseTimeoutSecs(const std::array<uint16_t, kIdentifyWordCount>& id, bool enhanced) {
    uint64_t estimate = (lbaSectors(id) / 2048ULL / 30ULL) + (30ULL * 60ULL);
    constexpr uint64_t kMaxEstimate = 12ULL * 60ULL * 60ULL;  // 12 hours
    if (estimate > kMaxEstimate) {
        estimate = kMaxEstimate;
    }

    const std::size_t idx = enhanced ? kEnhancedEraseTimeWord : kNormalEraseTimeWord;
    unsigned raw = id[idx];
    const bool ext = (raw & 0x8000u) != 0;  // bit 15 ⇒ extended (15-bit) time field
    raw = ext ? (raw & 0x7FFFu) : (raw & 0x00FFu);
    if (raw == 0) {
        return estimate;  // not specified by the drive
    }

    uint64_t minutes;
    if (ext && raw == 0x7FFFu) {
        minutes = 65532ULL + 90ULL;       // saturated extended value
    } else if (!ext && raw == 0x00FFu) {
        minutes = 508ULL + 90ULL;         // saturated non-extended value
    } else {
        minutes = static_cast<uint64_t>(raw) * 2ULL + 60ULL;  // value × 2 minutes + slack
    }
    const uint64_t timeout = minutes * 60ULL;
    return timeout < estimate ? estimate : timeout;
}

// Build the 512-byte SECURITY command data block: control word + 32-byte password.
void fillSecurityBuffer(uint8_t* buf, bool enhanced) {
    std::memset(buf, 0, 512);
    // word 0: bit 0 = identifier (0 = user), bit 1 = enhanced erase (ERASE UNIT only).
    buf[0] = enhanced ? 0x02 : 0x00;
    // words 1-16: password, NUL-padded (memset already zeroed the remainder).
    std::memcpy(buf + 2, kTempPassword, std::strlen(kTempPassword));
}

// Full hdparm-style ATA Secure Erase sequence. Throws std::runtime_error on any
// failure, attempting SECURITY DISABLE cleanup so the drive is never left locked.
void performSecureErase(const std::string& path, bool enhanced) {
    const auto id = readIdentifyData(path);  // throws on failure

    const uint16_t sec = id[kSecurityStatusWord];
    if ((sec & kSecSupported) == 0) {
        throw std::runtime_error("Secure Erase: ATA Security feature set not supported by " + path);
    }
    if ((sec & kSecFrozen) != 0) {
        throw std::runtime_error("Secure Erase: drive is frozen; a power cycle is required (" + path + ")");
    }
    if ((sec & (kSecEnabled | kSecLocked)) != 0) {
        throw std::runtime_error("Secure Erase: drive security is already enabled/locked (" + path + ")");
    }
    if (enhanced && (sec & kSecEnhancedSupport) == 0) {
        throw std::runtime_error("Enhanced Secure Erase is not supported by " + path);
    }

    const uint64_t timeoutSecs = eraseTimeoutSecs(id, enhanced);
    const uint64_t timeoutMs64 = timeoutSecs * 1000ULL;
    const unsigned timeoutMs =
        (timeoutMs64 > 0xFFFFFFFFULL) ? 0xFFFFFFFFu : static_cast<unsigned>(timeoutMs64);

    const int fd = ::open(path.c_str(), O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        throw std::runtime_error("Secure Erase: cannot open " + path);
    }

    std::array<uint8_t, 512> buf{};

    // 1) SECURITY SET PASSWORD — enables the security feature.
    fillSecurityBuffer(buf.data(), /*enhanced=*/false);
    if (!issueAtaCommand(fd, kAtaSecuritySetPassword, AtaDir::ToDevice, buf.data(), 1, kShortTimeoutMs)) {
        ::close(fd);
        throw std::runtime_error("Secure Erase: SECURITY SET PASSWORD failed on " + path);
    }

    // 2) SECURITY ERASE PREPARE — must immediately precede ERASE UNIT.
    const char* failedStep = nullptr;
    bool ok = issueAtaCommand(fd, kAtaSecurityErasePrepare, AtaDir::None, nullptr, 0, kShortTimeoutMs);
    if (!ok) {
        failedStep = "SECURITY ERASE PREPARE";
    } else {
        // 3) SECURITY ERASE UNIT — performs the wipe; auto-disables security on success.
        fillSecurityBuffer(buf.data(), enhanced);
        ok = issueAtaCommand(fd, kAtaSecurityEraseUnit, AtaDir::ToDevice, buf.data(), 1, timeoutMs);
        if (!ok) {
            failedStep = "SECURITY ERASE UNIT";
        }
    }

    if (!ok) {
        // Cleanup: leave the drive unlocked rather than password-enabled.
        fillSecurityBuffer(buf.data(), /*enhanced=*/false);
        issueAtaCommand(fd, kAtaSecurityDisable, AtaDir::ToDevice, buf.data(), 1, kShortTimeoutMs);
        ::close(fd);
        throw std::runtime_error(std::string("Secure Erase: ") + failedStep +
                                 " failed on " + path + " (attempted SECURITY DISABLE cleanup)");
    }

    ::close(fd);
}

}  // namespace

bool ATADisk::isFrozen() const {

    return true;
}

void ATADisk::unfreeze() {

}

ATADisk::~ATADisk() noexcept {
    if (blockFd_ >= 0) {
        ::close(blockFd_);
    }
}

int ATADisk::blockFd() {
    if (blockFd_ < 0) {
        blockFd_ = BlockIO::openDirect(getPath());
    }
    return blockFd_;
}

void ATADisk::writeBlock(uint64_t sectorOffset, const void* data, std::size_t dataSize) {
    BlockIO::writeAt(blockFd(), sectorOffset * getSectorSize(), data, dataSize);
}

void ATADisk::readBlock(uint64_t sectorOffset, void* data, std::size_t dataSize) {
    BlockIO::readAt(blockFd(), sectorOffset * getSectorSize(), data, dataSize);
}

void ATADisk::flush() {
    BlockIO::flush(blockFd_);
}

void ATADisk::secureErase(Callback callback) {
    callback(0.0);
    performSecureErase(getPath(), /*enhanced=*/false);
    callback(1.0);
}

void ATADisk::secureEraseEnhanced(Callback callback) {
    if (!supportsEnhancedErase()) {
        throw std::runtime_error("Enhanced Secure Erase is not supported for this disk");
    }
    callback(0.0);
    performSecureErase(getPath(), /*enhanced=*/true);
    callback(1.0);
}

bool ATADisk::supportsEnhancedErase() const {
    const std::array<uint16_t, kIdentifyWordCount> words = readIdentifyData(getPath());
    return (words[kSecurityStatusWord] & kSecEnhancedSupport) != 0;
}

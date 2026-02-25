
#include <stdexcept>
#include <operations.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <algorithm>
#include <random>

namespace DiskManagement {

static constexpr size_t BLOCK_SIZE = 1024 * 1024;     // 1 MiB
static constexpr size_t ALIGNMENT = 4096;             // Page alignment for O_DIRECT
static constexpr size_t RANDOM_REFILL_INTERVAL = 64;  // Refill random buffer every 64 blocks (64 MiB)

// Progress
Progress::Progress(
    unsigned long long bytesProcessed,
    unsigned long long totalBytes
) : _bytesProcessed(bytesProcessed), _totalBytes(totalBytes) {
    
}

unsigned long long Progress::bytesProcessed() const { return _bytesProcessed; }
unsigned long long Progress::totalBytes() const { return _totalBytes; }

double Progress::fractionCompleted() const {
    if (_totalBytes == 0) return 0.0;
    return static_cast<double>(_bytesProcessed) / static_cast<double>(_totalBytes);
}

double Progress::percentageCompleted() const {
    return fractionCompleted() * 100.0;
}

// Delegate
bool DiskDeleteMethod::Delegate::shouldCancel() { return false; }

// Constructor
DiskDeleteMethod::DiskDeleteMethod(const std::string& id) : id(id) {

}

const std::string& DiskDeleteMethod::getID() const {
    return id;
}

void DiskDeleteMethod::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) {
    delegate.onError(getTitle() + " does not support ATA disks");
}

void DiskDeleteMethod::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) {
    delegate.onError(getTitle() + " does not support NVMe disks");
}

void DiskDeleteMethod::deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) {
    delegate.onError(getTitle() + " does not support USB disks");
}

// Writable

void Writable::randomize(void* buffer, int urandom) {
    unsigned char* ptr = static_cast<unsigned char*>(buffer);
    size_t remaining = BLOCK_SIZE;
    while (remaining > 0) {
        ssize_t n = ::read(urandom, ptr, remaining);
        if (n < 0) {
            if (errno == EINTR) continue;
            throw std::runtime_error("Failed to read from /dev/urandom");
        }
        ptr += n;
        remaining -= n;
    }
}

void Writable::writeToDisk(Disk& disk, int diskFD, void* buffer, Callback callback,
                           std::function<void(void*, size_t)> refill) {
    unsigned long long bytesWritten = 0;
    size_t blocksSinceRefill = 0;

    while (bytesWritten < disk.getSize()) {
        size_t remaining = static_cast<size_t>(disk.getSize() - bytesWritten);
        size_t bytesToBeWritten = std::min(BLOCK_SIZE, remaining);

        if (refill && blocksSinceRefill >= RANDOM_REFILL_INTERVAL) {
            refill(buffer, BLOCK_SIZE);
            blocksSinceRefill = 0;
        }

        ssize_t nwritten;
        do {
            nwritten = ::write(diskFD, buffer, bytesToBeWritten);
        } while (nwritten < 0 && errno == EINTR);

        if (nwritten < 0) {
            if (errno == ENOSPC) break;
            int err = errno;
            throw std::runtime_error("Write failed on " + disk.getPath() + ": " + strerror(err));
        }

        bytesWritten += nwritten;
        blocksSinceRefill++;
        callback(Progress(bytesWritten, disk.getSize()));
    }

    if (fdatasync(diskFD) < 0) {
        int err = errno;
        throw std::runtime_error("fdatasync failed on " + disk.getPath() + ": " + strerror(err));
    }
}

void Writable::writeBytes(Disk& disk, void* buffer, Callback callback) {
    int diskFD = ::open(disk.getPath().c_str(), O_WRONLY | O_DIRECT);
    if (diskFD < 0) {
        free(buffer);
        throw std::runtime_error("Failed to open " + disk.getPath());
    }

    try {
        writeToDisk(disk, diskFD, buffer, callback, nullptr);
    } catch (...) {
        ::close(diskFD);
        free(buffer);
        throw;
    }

    ::close(diskFD);
    free(buffer);
}

void Writable::writeRandomBytes(Disk& disk, void* buffer, Callback callback) {
    int urandomFD = ::open("/dev/urandom", O_RDONLY);
    if (urandomFD < 0) {
        free(buffer);
        throw std::runtime_error("Failed to open /dev/urandom");
    }

    randomize(buffer, urandomFD);

    int diskFD = ::open(disk.getPath().c_str(), O_WRONLY | O_DIRECT);
    if (diskFD < 0) {
        ::close(urandomFD);
        free(buffer);
        throw std::runtime_error("Failed to open " + disk.getPath());
    }

    try {
        writeToDisk(disk, diskFD, buffer, callback, [urandomFD](void* buf, size_t) {
            randomize(buf, urandomFD);
        });
    } catch (...) {
        ::close(diskFD);
        ::close(urandomFD);
        free(buffer);
        throw;
    }

    ::close(diskFD);
    ::close(urandomFD);
    free(buffer);
}

void Writable::fillPattern(void* buffer, size_t bufferSize, std::span<const unsigned char> pattern) {
    auto* dst = static_cast<unsigned char*>(buffer);
    size_t patternSize = pattern.size();

    // Fill the first occurrence
    for (size_t i = 0; i < patternSize && i < bufferSize; ++i) {
        dst[i] = pattern[i];
    }

    // Double-copy to fill the rest efficiently
    for (size_t filled = patternSize; filled < bufferSize; filled *= 2) {
        size_t chunk = std::min(filled, bufferSize - filled);
        std::memcpy(dst + filled, dst, chunk);
    }
}

void Writable::write(Disk& disk, std::span<const unsigned char> pattern, Callback callback) const {
    void* buffer = nullptr;

    if (posix_memalign(&buffer, ALIGNMENT, BLOCK_SIZE)) {
        throw std::runtime_error("Failed to allocate aligned buffer");
    }

    fillPattern(buffer, BLOCK_SIZE, pattern);
    return writeBytes(disk, buffer, callback);
}

void Writable::write(Disk& disk, Method method, Callback callback) const {
    void* buffer = nullptr;

    if (posix_memalign(&buffer, ALIGNMENT, BLOCK_SIZE)) {
        throw std::runtime_error("Failed to allocate aligned buffer");
    }

    switch (method) {
    case Method::x0:
        memset(buffer, 0x00, BLOCK_SIZE);
        break;
    case Method::xFF:
        memset(buffer, 0xFF, BLOCK_SIZE);
        break;
    case Method::RANDOM:
        return writeRandomBytes(disk, buffer, callback);
    }

    return writeBytes(disk, buffer, callback);
}

// Verifiable

static std::vector<unsigned long long> generateSectorIndices(
    unsigned long long totalSectors,
    unsigned long long sectorsToVerify
) {
    // Full verification: sequential scan
    if (sectorsToVerify >= totalSectors) {
        std::vector<unsigned long long> indices(totalSectors);
        for (unsigned long long i = 0; i < totalSectors; i++) {
            indices[i] = i;
        }
        return indices;
    }

    // Partial sampling: generate random indices, sort, deduplicate
    std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<unsigned long long> dist(0, totalSectors - 1);

    // Over-generate slightly to account for duplicates after dedup
    unsigned long long toGenerate = sectorsToVerify + sectorsToVerify / 10;

    std::vector<unsigned long long> indices(toGenerate);
    for (unsigned long long i = 0; i < toGenerate; i++) {
        indices[i] = dist(rng);
    }

    std::sort(indices.begin(), indices.end());
    indices.erase(std::unique(indices.begin(), indices.end()), indices.end());

    // Trim to target count
    if (indices.size() > sectorsToVerify) {
        indices.resize(sectorsToVerify);
    }

    return indices;
}

VerificationResult Verifiable::verifySampling(
    const Disk& disk,
    double samplingPercentage,
    double passThreshold,
    Callback callback
) const {
    unsigned long long totalSectors = disk.getSectorCount();
    unsigned long long sectorsToVerify = static_cast<unsigned long long>(totalSectors * samplingPercentage);

    if (sectorsToVerify == 0) {
        sectorsToVerify = 1;
    }

    std::vector<unsigned long long> sectorIndices = generateSectorIndices(totalSectors, sectorsToVerify);
    unsigned long long actualCount = sectorIndices.size();

    // Allocate aligned buffer for O_DIRECT reading
    unsigned int sectorSize = disk.getSectorSize();
    size_t bufferSize = sectorSize;
    if (bufferSize < ALIGNMENT) {
        bufferSize = ALIGNMENT;
    }

    void* buffer = nullptr;
    if (posix_memalign(&buffer, ALIGNMENT, bufferSize)) {
        throw std::runtime_error("Failed to allocate aligned buffer for verification");
    }

    int fd = ::open(disk.getPath().c_str(), O_RDONLY | O_DIRECT);
    if (fd < 0) {
        free(buffer);
        throw std::runtime_error("Failed to open " + disk.getPath() + " for verification");
    }

    unsigned long long nonZeroSectors = 0;
    unsigned long long verified = 0;

    for (unsigned long long sectorIndex : sectorIndices) {
        off_t offset = static_cast<off_t>(sectorIndex) * sectorSize;

        if (::lseek(fd, offset, SEEK_SET) < 0) {
            ::close(fd);
            free(buffer);
            throw std::runtime_error("Failed to seek to sector " + std::to_string(sectorIndex));
        }

        std::memset(buffer, 0xFF, bufferSize);

        ssize_t bytesRead = ::read(fd, buffer, bufferSize);
        if (bytesRead < 0) {
            ::close(fd);
            free(buffer);
            throw std::runtime_error("Failed to read sector " + std::to_string(sectorIndex));
        }

        // Check if any byte in the sector is non-zero
        unsigned char* bytes = static_cast<unsigned char*>(buffer);
        for (unsigned int i = 0; i < sectorSize; i++) {
            if (bytes[i] != 0x00) {
                nonZeroSectors++;
                break;
            }
        }

        verified++;

        if (callback) {
            callback(Progress(verified, actualCount));
        }
    }

    ::close(fd);
    free(buffer);

    double nonZeroRatio = (actualCount > 0)
        ? static_cast<double>(nonZeroSectors) / static_cast<double>(actualCount)
        : 0.0;

    return VerificationResult {
        .sectorsVerified = verified,
        .sectorsTotal = totalSectors,
        .nonZeroSectors = nonZeroSectors,
        .samplingPercentage = samplingPercentage,
        .passed = nonZeroRatio < passThreshold
    };
}

VerificationResult Verifiable::verifyFull(
    const Disk& disk,
    Callback callback
) const {
    unsigned long long totalSectors = disk.getSectorCount();
    unsigned int sectorSize = disk.getSectorSize();

    void* buffer = nullptr;
    if (posix_memalign(&buffer, ALIGNMENT, BLOCK_SIZE)) {
        throw std::runtime_error("Failed to allocate aligned buffer for verification");
    }

    int fd = ::open(disk.getPath().c_str(), O_RDONLY | O_DIRECT);
    if (fd < 0) {
        free(buffer);
        throw std::runtime_error("Failed to open " + disk.getPath() + " for verification");
    }

    unsigned long long nonZeroSectors = 0;
    unsigned long long bytesVerified = 0;

    while (bytesVerified < disk.getSize()) {
        size_t toRead = (disk.getSize() - bytesVerified > BLOCK_SIZE)
            ? BLOCK_SIZE
            : static_cast<size_t>(disk.getSize() - bytesVerified);

        ssize_t bytesRead = ::read(fd, buffer, toRead);
        if (bytesRead <= 0) {
            if (bytesRead == 0) break;
            ::close(fd);
            free(buffer);
            throw std::runtime_error(
                "Verification read failed at offset " + std::to_string(bytesVerified));
        }

        // Count non-zero sectors within this block
        unsigned char* bytes = static_cast<unsigned char*>(buffer);
        for (ssize_t off = 0; off + sectorSize <= bytesRead; off += sectorSize) {
            for (unsigned int i = 0; i < sectorSize; i++) {
                if (bytes[off + i] != 0x00) {
                    nonZeroSectors++;
                    break;
                }
            }
        }

        bytesVerified += bytesRead;

        if (callback) {
            callback(Progress(bytesVerified, disk.getSize()));
        }
    }

    ::close(fd);
    free(buffer);

    unsigned long long sectorsVerified = bytesVerified / sectorSize;

    return VerificationResult {
        .sectorsVerified = sectorsVerified,
        .sectorsTotal = totalSectors,
        .nonZeroSectors = nonZeroSectors,
        .samplingPercentage = 1.0,
        .passed = nonZeroSectors == 0
    };
}

} // namespace DiskManagement
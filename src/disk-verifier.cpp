
#include <disk-verifier.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <vector>

static constexpr size_t ALIGNMENT = 4096;

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

    // Trim to target count (we may have slightly more after dedup of the over-generation)
    if (indices.size() > sectorsToVerify) {
        indices.resize(sectorsToVerify);
    }

    return indices;
}

VerificationResult DiskVerifier::verifySampling(
    const DiskManagement::Disk& disk,
    double samplingPercentage,
    double passThreshold,
    ProgressCallback onProgress
) {
    unsigned long long totalSectors = disk.getSectorCount();
    unsigned long long sectorsToVerify = static_cast<unsigned long long>(totalSectors * samplingPercentage);

    if (sectorsToVerify == 0) {
        sectorsToVerify = 1;
    }

    std::vector<unsigned long long> sectorIndices = generateSectorIndices(totalSectors, sectorsToVerify);
    unsigned long long actualCount = sectorIndices.size();

    // Allocate aligned buffer for O_DIRECT reading
    size_t bufferSize = static_cast<size_t>(disk.sectorSize);
    if (bufferSize < ALIGNMENT) {
        bufferSize = ALIGNMENT;
    }

    void* buffer = nullptr;
    if (posix_memalign(&buffer, ALIGNMENT, bufferSize)) {
        throw std::runtime_error("Failed to allocate aligned buffer for verification");
    }

    int fd = ::open(disk.path.c_str(), O_RDONLY | O_DIRECT);
    if (fd < 0) {
        free(buffer);
        throw std::runtime_error("Failed to open " + disk.path + " for verification");
    }

    unsigned long long nonZeroSectors = 0;
    unsigned long long verified = 0;

    for (unsigned long long sectorIndex : sectorIndices) {
        off_t offset = static_cast<off_t>(sectorIndex) * disk.sectorSize;

        if (::lseek(fd, offset, SEEK_SET) < 0) {
            ::close(fd);
            free(buffer);
            throw std::runtime_error("Failed to seek to sector " + std::to_string(sectorIndex));
        }

        memset(buffer, 0xFF, bufferSize);

        ssize_t bytesRead = ::read(fd, buffer, bufferSize);
        if (bytesRead < 0) {
            ::close(fd);
            free(buffer);
            throw std::runtime_error("Failed to read sector " + std::to_string(sectorIndex));
        }

        // Check if any byte in the sector is non-zero
        unsigned char* bytes = static_cast<unsigned char*>(buffer);
        for (int i = 0; i < disk.sectorSize; i++) {
            if (bytes[i] != 0x00) {
                nonZeroSectors++;
                break;
            }
        }

        verified++;

        if (onProgress) {
            onProgress(Progress(verified, actualCount));
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

VerificationResult DiskVerifier::verifyFull(
    const DiskManagement::Disk& disk,
    ProgressCallback onProgress
) {
    static constexpr size_t BLOCK_SIZE = 1024 * 1024; // 1 MiB — matches write block size

    unsigned long long totalSectors = disk.getSectorCount();

    void* buffer = nullptr;
    if (posix_memalign(&buffer, ALIGNMENT, BLOCK_SIZE)) {
        throw std::runtime_error("Failed to allocate aligned buffer for verification");
    }

    int fd = ::open(disk.path.c_str(), O_RDONLY | O_DIRECT);
    if (fd < 0) {
        free(buffer);
        throw std::runtime_error("Failed to open " + disk.path + " for verification");
    }

    unsigned long long nonZeroSectors = 0;
    unsigned long long bytesVerified = 0;

    while (bytesVerified < disk.size) {
        size_t toRead = (disk.size - bytesVerified > BLOCK_SIZE)
            ? BLOCK_SIZE
            : static_cast<size_t>(disk.size - bytesVerified);

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
        for (ssize_t off = 0; off + disk.sectorSize <= bytesRead; off += disk.sectorSize) {
            for (int i = 0; i < disk.sectorSize; i++) {
                if (bytes[off + i] != 0x00) {
                    nonZeroSectors++;
                    break;
                }
            }
        }

        bytesVerified += bytesRead;

        if (onProgress) {
            onProgress(Progress(bytesVerified, disk.size));
        }
    }

    ::close(fd);
    free(buffer);

    unsigned long long sectorsVerified = bytesVerified / disk.sectorSize;

    return VerificationResult {
        .sectorsVerified = sectorsVerified,
        .sectorsTotal = totalSectors,
        .nonZeroSectors = nonZeroSectors,
        .samplingPercentage = 1.0,
        .passed = nonZeroSectors == 0
    };
}

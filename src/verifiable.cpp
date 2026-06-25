
#include <verifiable.hpp>

#include <stdexcept>
#include <algorithm>
#include <cstdlib>

namespace {

constexpr std::size_t kChunkSize = 1024 * 1024;  // 1 MiB read chunk
constexpr std::size_t kAlignment = 4096;         // O_DIRECT page alignment

// True if every byte of [data, data+size) equals the repeating pattern.
// Note: compares with the pattern phase reset at `data` (correct for the
// single-byte patterns the standards verify; multi-byte pattern-verify is
// unused by the current methods, which all verify in integrity mode).
bool matchesPattern(const unsigned char* data, std::size_t size, std::span<const unsigned char> pattern) {
    const std::size_t patternSize = pattern.size();
    for (std::size_t i = 0; i < size; ++i) {
        if (data[i] != pattern[i % patternSize]) {
            return false;
        }
    }
    return true;
}

}  // namespace

Verifiable::VerificationResult
Verifiable::verify(std::optional<std::span<const unsigned char>> expected,
                   const ProgressCallback& onProgress) {
    const unsigned int sectorSize = getSectorSize();
    const unsigned long long sectorCount = getSectorCount();
    if (sectorSize == 0 || sectorCount == 0) {
        throw std::runtime_error("verify: disk reports zero geometry");
    }
    const unsigned long long totalBytes = sectorCount * sectorSize;

    std::size_t chunkBytes = kChunkSize - (kChunkSize % sectorSize);
    if (chunkBytes == 0) {
        chunkBytes = sectorSize;
    }

    void* raw = nullptr;
    if (posix_memalign(&raw, kAlignment, chunkBytes) != 0) {
        throw std::runtime_error("verify: failed to allocate aligned buffer");
    }
    auto* buf = static_cast<unsigned char*>(raw);

    unsigned long long bytesRead = 0;
    unsigned long long sectorsChecked = 0;
    unsigned long long mismatches = 0;
    try {
        while (bytesRead < totalBytes) {
            const unsigned long long remaining = totalBytes - bytesRead;
            const std::size_t thisChunk =
                static_cast<std::size_t>(std::min<unsigned long long>(chunkBytes, remaining));

            readBlock(bytesRead / sectorSize, buf, thisChunk);

            const std::size_t sectorsInChunk = thisChunk / sectorSize;
            if (expected) {
                for (std::size_t s = 0; s < sectorsInChunk; ++s) {
                    if (!matchesPattern(buf + s * sectorSize, sectorSize, *expected)) {
                        ++mismatches;
                    }
                }
            }
            sectorsChecked += sectorsInChunk;
            bytesRead += thisChunk;
            onProgress(static_cast<double>(bytesRead) / static_cast<double>(totalBytes));
        }
    } catch (...) {
        free(raw);
        throw;
    }
    free(raw);

    return VerificationResult{sectorsChecked, mismatches, mismatches == 0};
}

Verifiable::VerificationResult
Verifiable::verifySampling(double fraction,
                           std::optional<std::span<const unsigned char>> expected,
                           const ProgressCallback& onProgress) {
    const unsigned int sectorSize = getSectorSize();
    const unsigned long long sectorCount = getSectorCount();
    if (sectorSize == 0 || sectorCount == 0) {
        throw std::runtime_error("verifySampling: disk reports zero geometry");
    }
    fraction = std::clamp(fraction, 0.0, 1.0);

    // Sample uniformly across the disk by striding — bounded memory regardless
    // of disk size (no per-sector index vector).
    unsigned long long toSample =
        static_cast<unsigned long long>(static_cast<double>(sectorCount) * fraction);
    if (toSample == 0) {
        toSample = 1;
    }
    const unsigned long long stride = std::max<unsigned long long>(1, sectorCount / toSample);

    void* raw = nullptr;
    if (posix_memalign(&raw, kAlignment, sectorSize) != 0) {
        throw std::runtime_error("verifySampling: failed to allocate aligned buffer");
    }
    auto* buf = static_cast<unsigned char*>(raw);

    unsigned long long sectorsChecked = 0;
    unsigned long long mismatches = 0;
    try {
        for (unsigned long long sector = 0; sector < sectorCount; sector += stride) {
            readBlock(sector, buf, sectorSize);
            if (expected && !matchesPattern(buf, sectorSize, *expected)) {
                ++mismatches;
            }
            ++sectorsChecked;
            onProgress(static_cast<double>(sector) / static_cast<double>(sectorCount));
        }
    } catch (...) {
        free(raw);
        throw;
    }
    free(raw);

    onProgress(1.0);
    return VerificationResult{sectorsChecked, mismatches, mismatches == 0};
}

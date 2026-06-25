
#include <block_writable.hpp>

#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <cerrno>

#include <fcntl.h>
#include <unistd.h>

namespace {

constexpr std::size_t kChunkSize = 1024 * 1024;   // 1 MiB write chunk
constexpr std::size_t kAlignment = 4096;          // O_DIRECT page alignment
constexpr std::size_t kRandomRefillChunks = 64;   // refresh random data every 64 MiB

// Fill `buf` by repeating `pattern`, doubling the filled region each step.
void fillPattern(unsigned char* buf, std::size_t bufferSize, std::span<const unsigned char> pattern) {
    if (pattern.empty()) {
        std::memset(buf, 0, bufferSize);  // guard against an empty pattern (no infinite loop)
        return;
    }
    const std::size_t prefix = std::min(pattern.size(), bufferSize);
    std::memcpy(buf, pattern.data(), prefix);
    for (std::size_t filled = prefix; filled < bufferSize; ) {
        const std::size_t chunk = std::min(filled, bufferSize - filled);
        std::memcpy(buf + filled, buf, chunk);
        filled += chunk;
    }
}

void readRandom(int urandomFd, unsigned char* buf, std::size_t size) {
    std::size_t remaining = size;
    unsigned char* p = buf;
    while (remaining > 0) {
        const ssize_t n = ::read(urandomFd, p, remaining);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw std::runtime_error("overwrite: failed to read /dev/urandom");
        }
        p += n;
        remaining -= static_cast<std::size_t>(n);
    }
}

// Shared full-disk overwrite driver used by both public overloads.
void runOverwrite(BlockWritable& dev, std::span<const unsigned char> pattern, bool random,
                  const BlockWritable::ProgressCallback& onProgress) {
    const unsigned int sectorSize = dev.getSectorSize();
    const unsigned long long sectorCount = dev.getSectorCount();
    if (sectorSize == 0 || sectorCount == 0) {
        throw std::runtime_error("overwrite: disk reports zero geometry");
    }
    const unsigned long long totalBytes = sectorCount * sectorSize;

    // Chunk size: 1 MiB rounded down to a whole number of sectors (>= one sector),
    // so every writeBlock length is sector-aligned for O_DIRECT.
    std::size_t chunkBytes = kChunkSize - (kChunkSize % sectorSize);
    if (chunkBytes == 0) {
        chunkBytes = sectorSize;
    }

    void* raw = nullptr;
    if (posix_memalign(&raw, kAlignment, chunkBytes) != 0) {
        throw std::runtime_error("overwrite: failed to allocate aligned buffer");
    }
    auto* buf = static_cast<unsigned char*>(raw);

    int urandomFd = -1;
    try {
        if (random) {
            urandomFd = ::open("/dev/urandom", O_RDONLY);
            if (urandomFd < 0) {
                throw std::runtime_error("overwrite: failed to open /dev/urandom");
            }
            readRandom(urandomFd, buf, chunkBytes);
        } else {
            fillPattern(buf, chunkBytes, pattern);
        }

        unsigned long long bytesWritten = 0;
        std::size_t chunksSinceRefill = 0;
        while (bytesWritten < totalBytes) {
            const unsigned long long remaining = totalBytes - bytesWritten;
            const std::size_t thisChunk =
                static_cast<std::size_t>(std::min<unsigned long long>(chunkBytes, remaining));

            if (random && chunksSinceRefill >= kRandomRefillChunks) {
                readRandom(urandomFd, buf, chunkBytes);
                chunksSinceRefill = 0;
            }

            dev.writeBlock(bytesWritten / sectorSize, buf, thisChunk);

            bytesWritten += thisChunk;
            ++chunksSinceRefill;
            onProgress(static_cast<double>(bytesWritten) / static_cast<double>(totalBytes));
        }

        dev.flush();
    } catch (...) {
        if (urandomFd >= 0) {
            ::close(urandomFd);
        }
        free(raw);
        throw;
    }

    if (urandomFd >= 0) {
        ::close(urandomFd);
    }
    free(raw);
}

}  // namespace

void BlockWritable::overwrite(Fill fill, const ProgressCallback& onProgress) {
    switch (fill) {
        case Fill::Zero: {
            const unsigned char value = 0x00;
            runOverwrite(*this, std::span<const unsigned char>(&value, 1), false, onProgress);
            break;
        }
        case Fill::Ones: {
            const unsigned char value = 0xFF;
            runOverwrite(*this, std::span<const unsigned char>(&value, 1), false, onProgress);
            break;
        }
        case Fill::Random:
            runOverwrite(*this, {}, true, onProgress);
            break;
    }
}

void BlockWritable::overwrite(std::span<const unsigned char> pattern, const ProgressCallback& onProgress) {
    runOverwrite(*this, pattern, false, onProgress);
}

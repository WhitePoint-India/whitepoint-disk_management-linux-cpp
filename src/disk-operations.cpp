
#include <disk-operations.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <algorithm>
#include <stdexcept>

static constexpr size_t BLOCK_SIZE = 1024 * 1024;    // 1 MiB (bs=1M)
static constexpr size_t ALIGNMENT = 4096;            // Page alignment for O_DIRECT
static constexpr size_t RANDOM_REFILL_INTERVAL = 64; // Refill random buffer every 64 blocks (64 MiB)

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

void Writable::writeToDisk(DiskManagement::Disk& disk, int diskFD, void* buffer, Callback callback,
                           std::function<void(void*, size_t)> refill) {
    unsigned long long bytesWritten = 0;
    size_t blocksSinceRefill = 0;

    while (bytesWritten < disk.size) {
        size_t remaining = static_cast<size_t>(disk.size - bytesWritten);
        size_t bytesToBeWritten = std::min(BLOCK_SIZE, remaining);

        // Refill buffer periodically if a refill function is provided
        if (refill && blocksSinceRefill >= RANDOM_REFILL_INTERVAL) {
            refill(buffer, BLOCK_SIZE);
            blocksSinceRefill = 0;
        }

        ssize_t nwritten;
        do {
            nwritten = ::write(diskFD, buffer, bytesToBeWritten);
        }
        while (nwritten < 0 && errno == EINTR);

        if (nwritten < 0) {
            if (errno == ENOSPC) break; // Disk full — expected when writing entire device
            int err = errno;
            throw std::runtime_error("Write failed on " + disk.path + ": " + strerror(err));
        }

        bytesWritten += nwritten;
        blocksSinceRefill++;
        callback(Progress(bytesWritten, disk.size));
    }

    // Flush data to physical media before verification can begin.
    // O_DIRECT bypasses the OS page cache but does NOT guarantee data has
    // passed through the drive's internal volatile write cache.
    if (fdatasync(diskFD) < 0) {
        int err = errno;
        throw std::runtime_error("fdatasync failed on " + disk.path + ": " + strerror(err));
    }
}

void Writable::writeBytes(DiskManagement::Disk& disk, void* buffer, Callback callback) {
    // Open 'disk.path' for O_DIRECT write operation
    int diskFD = ::open(disk.path.c_str(), O_WRONLY | O_DIRECT);
    if (diskFD < 0) {
        free(buffer);
        throw std::runtime_error("Failed to open " + disk.path + ".");
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

void Writable::writeRandomBytes(DiskManagement::Disk& disk, void* buffer, Callback callback) {
    // Open '/dev/urandom' to generate random bytes
    int urandomFD = ::open("/dev/urandom", O_RDONLY);
    if (urandomFD < 0) {
        free(buffer);
        throw std::runtime_error("Failed to open /dev/urandom");
    }
    // Initialize buffer with random bytes
    randomize(buffer, urandomFD);
    // Open 'disk.path' for O_DIRECT write operation
    int diskFD = ::open(disk.path.c_str(), O_WRONLY | O_DIRECT);
    if (diskFD < 0) {
        ::close(urandomFD);
        free(buffer);
        throw std::runtime_error("Failed to open " + disk.path + ".");
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

void Writable::write(DiskManagement::Disk& disk, Method method, Callback callback) const {

    /// Buffer to store what to write
    void* buffer = nullptr;

    /// MARK: Allocate a memory for the `buffer` which is page aligned for `O_DIRECT` write operation.
    if (posix_memalign(&buffer, ALIGNMENT, BLOCK_SIZE)) { throw std::runtime_error("Failed to allocate aligned buffer"); }

    switch (method) {
    case Writable::Method::x0:
        memset(buffer, 0x00, BLOCK_SIZE);
        break;
    case Writable::Method::xFF:
        memset(buffer, 0xFF, BLOCK_SIZE);
        break;
    case Writable::Method::RANDOM:
        return writeRandomBytes(disk, buffer, callback);
    }

    return writeBytes(disk, buffer, callback);
}


#include <block_io.hpp>

#include <stdexcept>
#include <system_error>
#include <cerrno>

#include <fcntl.h>
#include <unistd.h>

namespace BlockIO {

int openDirect(const std::string& path) {
    const int fd = ::open(path.c_str(), O_RDWR | O_DIRECT);
    if (fd < 0) {
        throw std::system_error(errno, std::generic_category(), "open " + path);
    }
    return fd;
}

void writeAt(int fd, uint64_t byteOffset, const void* data, std::size_t size) {
    const auto* p = static_cast<const unsigned char*>(data);
    std::size_t remaining = size;
    off_t offset = static_cast<off_t>(byteOffset);
    while (remaining > 0) {
        const ssize_t n = ::pwrite(fd, p, remaining, offset);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw std::system_error(errno, std::generic_category(), "pwrite");
        }
        if (n == 0) {
            throw std::runtime_error("pwrite wrote 0 bytes");
        }
        p += n;
        remaining -= static_cast<std::size_t>(n);
        offset += n;
    }
}

void readAt(int fd, uint64_t byteOffset, void* data, std::size_t size) {
    auto* p = static_cast<unsigned char*>(data);
    std::size_t remaining = size;
    off_t offset = static_cast<off_t>(byteOffset);
    while (remaining > 0) {
        const ssize_t n = ::pread(fd, p, remaining, offset);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw std::system_error(errno, std::generic_category(), "pread");
        }
        if (n == 0) {
            throw std::runtime_error("pread hit unexpected end of device");
        }
        p += n;
        remaining -= static_cast<std::size_t>(n);
        offset += n;
    }
}

void flush(int fd) {
    if (fd >= 0 && ::fdatasync(fd) < 0) {
        throw std::system_error(errno, std::generic_category(), "fdatasync");
    }
}

}  // namespace BlockIO

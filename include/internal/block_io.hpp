
#ifndef BLOCK_IO_HPP
#define BLOCK_IO_HPP

#include <string>
#include <cstddef>
#include <cstdint>

// Positioned O_DIRECT block I/O helpers shared by the concrete disk types.
// Callers must pass aligned buffers, sector-multiple sizes, and sector-aligned
// byte offsets (the overwrite/verify engines guarantee this).
namespace BlockIO {

[[nodiscard]] int openDirect(const std::string& path);  // O_RDWR | O_DIRECT; throws std::system_error
void writeAt(int fd, uint64_t byteOffset, const void* data, std::size_t size);
void readAt(int fd, uint64_t byteOffset, void* data, std::size_t size);
void flush(int fd);  // fdatasync; no-op if fd < 0

}  // namespace BlockIO

#endif // BLOCK_IO_HPP


#include <string>
#include <cerrno>
#include <chrono>
#include <thread>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <endian.h>
#include <unistd.h>
#include <stdexcept>
#include <libnvme.h>
#include <system_error>
#include <nvme_disk.hpp>
#include <block_io.hpp>

// libnvme return codes: rc < 0 is a transport error (errno set), rc > 0 is an
// NVMe status code.

NVMeDisk::~NVMeDisk() noexcept {
    if (blockFd_ >= 0) {
        ::close(blockFd_);
    }
}

int NVMeDisk::blockFd() {
    if (blockFd_ < 0) {
        blockFd_ = BlockIO::openDirect(getPath());
    }
    return blockFd_;
}

void NVMeDisk::writeBlock(uint64_t sectorOffset, const void* data, std::size_t dataSize) {
    BlockIO::writeAt(blockFd(), sectorOffset * getSectorSize(), data, dataSize);
}

void NVMeDisk::readBlock(uint64_t sectorOffset, void* data, std::size_t dataSize) {
    BlockIO::readAt(blockFd(), sectorOffset * getSectorSize(), data, dataSize);
}

void NVMeDisk::flush() {
    BlockIO::flush(blockFd_);
}

void NVMeDisk::sanitize(Action action, Callback callback) {
    const int fileDescriptor = ::open(getPath().c_str(), O_RDWR | O_NONBLOCK);
    if (fileDescriptor < 0) {
        throw std::system_error(errno, std::generic_category(), "open " + getPath());
    }
    struct nvme_sanitize_nvm_args args = {
        .args_size = static_cast<int>(sizeof(args)),
        .fd        = fileDescriptor,
        .sanact    = static_cast<enum nvme_sanitize_sanact>(static_cast<int>(action)),
    };
    int result = nvme_sanitize_nvm(&args);
    if (result < 0) {
        ::close(fileDescriptor);
        throw std::system_error(errno, std::generic_category(), "NVMe sanitize start failed");
    }
    else if (result > 0) {
        ::close(fileDescriptor);
        throw std::runtime_error("NVMe sanitize start failed: NVMe status 0x" + std::to_string(result));
    }
    else {
        // Sanitize started successfully. The operation runs in the background and cannot be aborted.
        callback(0.0);
        // The progress is reported by polling the Sanitize Status log (LID 81h) until it leaves the in-progress state.
        while (true) {
            struct nvme_sanitize_log_page log;
            int result = nvme_get_log_sanitize(fileDescriptor, false, &log);
            if (result < 0) {
                ::close(fileDescriptor);
                throw std::system_error(errno, std::generic_category(), "NVMe sanitize status read failed");
            }
            else if (result > 0) {
                ::close(fileDescriptor);
                throw std::runtime_error("NVMe sanitize status read failed: NVMe status 0x" + std::to_string(result));
            }
            else {
                const uint16_t sstat  = le16toh(log.sstat);
                const uint16_t status = static_cast<uint16_t>(sstat & NVME_SANITIZE_SSTAT_STATUS_MASK);
                if (status == NVME_SANITIZE_SSTAT_STATUS_COMPLETED_FAILED) {
                    ::close(fileDescriptor);
                    throw std::runtime_error("NVMe sanitize failed (sstat status = completed-failed)");
                }
                if (status != NVME_SANITIZE_SSTAT_STATUS_IN_PROGESS) {
                    break;  // completed successfully (or nothing in progress)
                }
                // SPROG is the progress out of 65536 (NVMe Sanitize Status log).
                const uint16_t sprog = le16toh(log.sprog);
                callback(static_cast<double>(sprog) / 65536.0);
                // Sleep for a second before polling again. The NVMe spec does not specify a minimum interval, but 1 second is reasonable to avoid excessive polling.
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        // Sanitize completed successfully. Close the file descriptor and report 100% completion.
        ::close(fileDescriptor);
        callback(1.0);
    }
}

void NVMeDisk::formatNVM(Callback callback) {
    const int fileDescriptor = ::open(getPath().c_str(), O_RDWR | O_NONBLOCK);
    if (fileDescriptor < 0) {
        throw std::system_error(errno, std::generic_category(), "open " + getPath());
    }

    // Format all namespaces with a user-data erase, keeping LBA format 0.
    struct nvme_format_nvm_args args = {
        .args_size = static_cast<int>(sizeof(args)),
        .fd        = fileDescriptor,
        .timeout   = 600000,  // 10 min; a format with erase can be slow
        .nsid      = NVME_NSID_ALL,
        .ses       = NVME_FORMAT_SES_USER_DATA_ERASE,
    };

    // Format NVM is a synchronous command with no progress log, so progress is
    // coarse: 0% before issuing and 100% once it returns.
    callback(0.0);
    const int rc = nvme_format_nvm(&args);
    if (rc != 0) {
        ::close(fileDescriptor);
        throw std::runtime_error("NVMe format failed: " +
            (rc < 0 ? std::string(std::strerror(errno)) : "NVMe status 0x" + std::to_string(rc)));
    }
    ::close(fileDescriptor);
    callback(1.0);
}

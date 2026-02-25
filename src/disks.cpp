

#include <fcntl.h>
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <disks.hpp>
#include <sys/ioctl.h>
#include <stdexcept>
#include <system_error>
#include <linux/hdreg.h>

Disks::ATADisk::ATADisk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    unsigned int sectorSize
) : Disk(
        serial,
        model,
        path,
        description,
        size,
        sectorSize
    ) {
}

bool Disks::ATADisk::isFrozen() const {

    int fileDescriptor = open(getPath().c_str(), O_RDONLY | O_NONBLOCK);

    if (fileDescriptor < 0) {
        throw std::system_error(errno, std::generic_category(), "Failed to open " + getPath());
    }

    unsigned char buffer [4 + 512] = { 0 };

    buffer[0] = 0xEC;
    buffer[3] = 1;

    if (ioctl(fileDescriptor, HDIO_DRIVE_CMD, buffer) < 0) {
        int err = errno;
        close(fileDescriptor);
        throw std::system_error(err, std::generic_category(), "IDENTIFY DEVICE ioctl failed for " + getPath());
    }

    close(fileDescriptor);

    uint16_t securityStatus;

    memcpy(&securityStatus, buffer + 4 + (128 * 2), sizeof(uint16_t));
    
    // Bit 3: frozen
    return (securityStatus >> 3) & 1;
}

void Disks::ATADisk::unfreeze() {
    throw std::runtime_error("unfreeze() is not yet implemented");
}

Disks::NVMeDisk::NVMeDisk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    unsigned int sectorSize
) : Disk(
        serial,
        model,
        path,
        description,
        size,
        sectorSize
    ) {
}

Disks::USBDisk::USBDisk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    unsigned int sectorSize
) : Disk(
        serial,
        model,
        path,
        description,
        size,
        sectorSize
    ) {
}
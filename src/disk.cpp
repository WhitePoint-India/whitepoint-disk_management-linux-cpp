
#include <cstdint>
#include <cstring>
#include <system_error>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>
#include <disk-management>

DiskManagement::Disk::Disk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    int sectorSize
) : serial(serial),
    model(model), 
    path(path),
    description(description),
    size(size),
    sectorSize(sectorSize) {
}

unsigned long long DiskManagement::Disk::getSectorCount() const {
    return size / sectorSize;
}

DiskManagement::ATADisk::ATADisk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    int sectorSize
) : Disk(
        serial,
        model,
        path,
        description,
        size,
        sectorSize
    ) {
}

bool DiskManagement::ATADisk::isFrozen() {

    int fileDescriptor = open(path.c_str(), O_RDONLY | O_NONBLOCK);
    
    if (fileDescriptor < 0) {
        throw std::system_error(errno, std::generic_category(), "Failed to open " + path);
    }

    unsigned char buffer [4 + 512] = { 0 };

    buffer[0] = 0xEC;
    buffer[3] = 1;

    if (ioctl(fileDescriptor, HDIO_DRIVE_CMD, buffer) < 0) {
        int err = errno;
        close(fileDescriptor);
        throw std::system_error(err, std::generic_category(), "IDENTIFY DEVICE ioctl failed for " + path);
    }

    close(fileDescriptor);

    uint16_t securityStatus;

    memcpy(&securityStatus, buffer + 4 + (128 * 2), sizeof(uint16_t));
    
    // Bit 3: frozen
    return (securityStatus >> 3) & 1;
}

void DiskManagement::ATADisk::unfreeze() {
    
}

DiskManagement::NVMeDisk::NVMeDisk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    int sectorSize
) : Disk(
        serial,
        model,
        path,
        description,
        size,
        sectorSize
    ) {
}

DiskManagement::USBDisk::USBDisk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    int sectorSize
) : Disk(
        serial,
        model,
        path,
        description,
        size,
        sectorSize
    ) {
}
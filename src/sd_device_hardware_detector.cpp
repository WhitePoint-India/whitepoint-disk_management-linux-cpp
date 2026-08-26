#include "sd_device_hardware_detector.hpp"
#include "disk.hpp"
#include "ata_disk.hpp"
#include "nvme_disk.hpp"

#include <cstring>
#include <cstdlib>
#include <climits>
#include <iostream>
#include <optional>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <systemd/sd-device.h>

namespace {

using EnumeratorPtr = std::unique_ptr<sd_device_enumerator, decltype(&sd_device_enumerator_unref)>;

// "removable" fails open (treated as not removable) if it can't be read;
// only an explicit "1" marks hot-pluggable media (USB, SD/MMC, etc.).
bool is_removable(sd_device *dev) {
    const char *removable_val = nullptr;
    return sd_device_get_sysattr_value(dev, "removable", &removable_val) >= 0
           && removable_val && strcmp(removable_val, "1") == 0;
}

bool is_internal_disk(sd_device *dev, const std::string &devnode) {
    if (devnode.find("/dev/sr") != std::string::npos) {
        return false; // CD/DVD optical drives aren't disks we care about
    }
    return !is_removable(dev);
}

// std::nullopt for anything that isn't NVMe or SATA (USB, SCSI, MMC, virtio,
// etc.) — BusType has no catch-all value, so those disks are filtered out by
// the caller instead of being force-fit into one of the two categories.
std::optional<BusType> detect_bus_type(sd_device *dev, const std::string &devnode) {
    const char *bus_val = nullptr;
    sd_device_get_property_value(dev, "ID_BUS", &bus_val);
    if (bus_val && strcmp(bus_val, "ata") == 0) {
        return BusType::SATA;
    }

    // ID_BUS is never set for NVMe, so fall back to the device name prefix,
    // mirroring lsblk's own last-resort fallback in get_transport()
    // (util-linux lsblk-cmd/lsblk.c).
    std::string name = devnode.substr(devnode.find_last_of('/') + 1);
    if (name.rfind("nvme", 0) == 0) {
        return BusType::NVMe;
    }

    return std::nullopt;
}

// /sys/block/<dev>/size is always expressed in 512-byte units by the kernel
// (same convention `blockdev --getsz` uses), regardless of the device's
// actual logical sector size — so convert to bytes here rather than handing
// back a raw sector count that callers could multiply by the wrong sector
// size (that previously inflated the reported capacity on 4Kn drives, whose
// logical_block_size is 4096 rather than 512).
unsigned long long get_total_bytes(sd_device *dev, const std::string &devnode) {
    const char *sector_val = nullptr;
    if (sd_device_get_sysattr_value(dev, "size", &sector_val) >= 0 && sector_val) {
        char *end = nullptr;
        unsigned long long sectors_512 = strtoull(sector_val, &end, 10);
        if (end != sector_val) {
            return sectors_512 * 512ULL;
        }
    }

    int fd = open(devnode.c_str(), O_RDONLY);
    if (fd < 0) {
        return 0;
    }
    unsigned long long byte_size = 0;
    ioctl(fd, BLKGETSIZE64, &byte_size);
    close(fd);
    return byte_size;
}

unsigned int get_sector_size(sd_device *dev) {
    const char *sector_size_val = nullptr;
    if (sd_device_get_sysattr_value(dev, "queue/logical_block_size", &sector_size_val) >= 0 && sector_size_val) {
        char *end = nullptr;
        unsigned long sector_size = strtoul(sector_size_val, &end, 10);
        if (end != sector_size_val && sector_size <= static_cast<unsigned long>(UINT_MAX)) {
            return static_cast<unsigned int>(sector_size);
        }
    }
    return 512;
}

} // namespace

std::vector<std::unique_ptr<Disk>> SDDeviceHardwareDetector::fetchDisks() {
    std::vector<std::unique_ptr<Disk>> disks;

    sd_device_enumerator *enumerator_raw = nullptr;
    if (sd_device_enumerator_new(&enumerator_raw) < 0) {
        std::cerr << "sd-device: failed to create device enumerator\n";
        return disks;
    }
    // RAII wrapper guarantees the enumerator (and the devices it owns) is
    // released on every exit path, including the early returns below.
    EnumeratorPtr enumerator(enumerator_raw, sd_device_enumerator_unref);

    if (sd_device_enumerator_add_match_subsystem(enumerator.get(), "block", 1) < 0 ||
        sd_device_enumerator_add_match_property(enumerator.get(), "DEVTYPE", "disk") < 0) {
        std::cerr << "sd-device: failed to set enumerator match rules\n";
        return disks;
    }

    sd_device *dev = sd_device_enumerator_get_device_first(enumerator.get());
    while (dev != nullptr) {
        // Advance to the next device up front so a `continue` below always
        // moves the loop forward instead of re-checking the same device.
        sd_device *current = dev;
        dev = sd_device_enumerator_get_device_next(enumerator.get());

        const char *syspath = nullptr;
        if (sd_device_get_syspath(current, &syspath) < 0 || strstr(syspath, "/sys/devices/virtual/") != nullptr) {
            continue;
        }

        const char *devnode_c = nullptr;
        if (sd_device_get_devname(current, &devnode_c) < 0 || !devnode_c) {
            continue;
        }
        std::string devnode(devnode_c);

        if (!is_internal_disk(current, devnode)) {
            continue;
        }

        auto bustype = detect_bus_type(current, devnode);
        if (!bustype) {
            continue; // only NVMe/SATA disks are represented by BusType
        }

        //Model
        const char *model_val = nullptr;
        sd_device_get_property_value(current, "ID_MODEL", &model_val);
        if (!model_val) {
            sd_device_get_sysattr_value(current, "/device/model", &model_val);
        }

        //Serial
        const char *serial_val = nullptr;
        sd_device_get_property_value(current, "ID_SERIAL_SHORT", &serial_val);
        if (!serial_val) {
            sd_device_get_sysattr_value(current, "/device/serial", &serial_val);
        }

        const unsigned int sector_size = get_sector_size(current);
        const unsigned long long size = get_total_bytes(current, devnode);
        if (size == 0) {
            continue;
        }

        const std::string model = model_val ? model_val : "UNKNOWN";
        const std::string serial = serial_val ? serial_val : "UNKNOWN";
        const std::string description = *bustype == BusType::NVMe ? "NVMe disk" : "ATA Disk";

        if (*bustype == BusType::NVMe) {
            disks.push_back(std::make_unique<NVMeDisk>(
                serial, model, devnode, description, *bustype, size, sector_size));
        }
        else {
            disks.push_back(std::make_unique<ATADisk>(
                serial, model, devnode, description, *bustype, size, sector_size));
        }
    }

    return disks;
}

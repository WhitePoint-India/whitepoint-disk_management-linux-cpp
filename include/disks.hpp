#ifndef DISKS_HPP
#define DISKS_HPP

#include <variant>

#include <ata_disk.hpp>
#include <nvme_disk.hpp>

using DiskVariant = std::variant<ATADisk, NVMeDisk>;

#endif // DISKS_HPP

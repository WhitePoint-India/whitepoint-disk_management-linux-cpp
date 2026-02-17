#ifndef DISK_H
#define DISK_H

#include <string>

namespace DiskManagement {

class Disk {
public:
    std::string serial;
    std::string model;
    std::string path;
    std::string description;
    unsigned long long size;
    int sectorSize;

    Disk(
        const std::string& serial,
        const std::string& model,
        const std::string& path,
        const std::string& description,
        unsigned long long size,
        int sectorSize
    );

    virtual ~Disk() = default;

    unsigned long long getSectorCount() const;
};

} // namespace DiskManagement

#endif // DISK_H

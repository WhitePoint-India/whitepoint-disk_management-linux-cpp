#ifndef DISK_H
#define DISK_H

#include <string>

class Disk {
private:
    std::string serial_;
    std::string model_;
    std::string path_;
    std::string description_;
    unsigned long long size_;
    unsigned int sectorSize_;

public:
    Disk(
        const std::string& serial,
        const std::string& model,
        const std::string& path,
        const std::string& description,
        unsigned long long size,
        unsigned int sectorSize
    );

    virtual ~Disk() = default;

    [[nodiscard]] const std::string& getSerial() const;
    [[nodiscard]] const std::string& getModel() const;
    [[nodiscard]] const std::string& getPath() const;
    [[nodiscard]] const std::string& getDescription() const;
    [[nodiscard]] unsigned long long getSize() const;
    [[nodiscard]] unsigned int getSectorSize() const;
    [[nodiscard]] unsigned long long getSectorCount() const;
};

#endif // DISK_H

#ifndef DISK_HPP
#define DISK_HPP

#include <string>

enum class BusType {
    NVMe,
    SATA
};

class Disk {
private:
    std::string serial_;
    std::string model_;
    std::string path_;
    std::string description_;
    BusType busType_;
    unsigned long long size_;
    unsigned int sectorSize_;

public:
    Disk(
        std::string serial,
        std::string model,
        std::string path,
        std::string description,
        BusType busType,
        unsigned long long size,
        unsigned int sectorSize
    );

    virtual ~Disk() noexcept = default;

    [[nodiscard]] const std::string& getSerial() const noexcept;
    [[nodiscard]] const std::string& getModel() const noexcept;
    [[nodiscard]] const std::string& getPath() const noexcept;
    [[nodiscard]] const std::string& getDescription() const noexcept;
    [[nodiscard]] BusType getBusType() const noexcept;
    [[nodiscard]] unsigned long long getSize() const noexcept;
    [[nodiscard]] virtual unsigned int getSectorSize() const noexcept;
    [[nodiscard]] virtual unsigned long long getSectorCount() const noexcept;
};

#endif // DISK_HPP

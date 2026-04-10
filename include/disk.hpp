#ifndef DISK_HPP
#define DISK_HPP

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
        std::string serial,
        std::string model,
        std::string path,
        std::string description,
        unsigned long long size,
        unsigned int sectorSize
    );

    virtual ~Disk() noexcept = default;

    [[nodiscard]] const std::string& getSerial() const noexcept;
    [[nodiscard]] const std::string& getModel() const noexcept;
    [[nodiscard]] const std::string& getPath() const noexcept;
    [[nodiscard]] const std::string& getDescription() const noexcept;
    [[nodiscard]] unsigned long long getSize() const noexcept;
    [[nodiscard]] unsigned int getSectorSize() const noexcept;
    [[nodiscard]] unsigned long long getSectorCount() const noexcept;
};

#endif // DISK_HPP

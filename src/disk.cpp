#include <utility>
#include <disk.hpp>

Disk::Disk(
    std::string serial,
    std::string model,
    std::string path,
    std::string description,
    BusType busType,
    unsigned long long size,
    unsigned int sectorSize
) : serial_(std::move(serial)),
    model_(std::move(model)),
    path_(std::move(path)),
    description_(std::move(description)),
    busType_(busType),
    size_(size),
    sectorSize_(sectorSize) {
}

const std::string& Disk::getSerial() const noexcept { return serial_; }
const std::string& Disk::getModel() const noexcept { return model_; }
const std::string& Disk::getPath() const noexcept { return path_; }
const std::string& Disk::getDescription() const noexcept { return description_; }
BusType Disk::getBusType() const noexcept { return busType_; }
unsigned long long Disk::getSize() const noexcept { return size_; }
unsigned int Disk::getSectorSize() const noexcept { return sectorSize_; }

unsigned long long Disk::getSectorCount() const noexcept {
    if (sectorSize_ == 0) {
        return 0;
    }
    return size_ / sectorSize_;
}

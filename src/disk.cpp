
#include <disk.hpp>

Disk::Disk(
    const std::string& serial,
    const std::string& model,
    const std::string& path,
    const std::string& description,
    unsigned long long size,
    unsigned int sectorSize
) : serial_(serial),
    model_(model),
    path_(path),
    description_(description),
    size_(size),
    sectorSize_(sectorSize) {
}

const std::string& Disk::getSerial() const { return serial_; }
const std::string& Disk::getModel() const { return model_; }
const std::string& Disk::getPath() const { return path_; }
const std::string& Disk::getDescription() const { return description_; }
unsigned long long Disk::getSize() const { return size_; }
unsigned int Disk::getSectorSize() const { return sectorSize_; }

unsigned long long Disk::getSectorCount() const {
    if (sectorSize_ == 0) {
        return 0;
    }
    return size_ / sectorSize_;
}

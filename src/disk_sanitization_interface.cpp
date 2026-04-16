
#include <disk_sanitization_interface.hpp>

DiskSanitizationInterface::DiskSanitizationInterface(std::string key) : key_(std::move(key)) {
}

const std::string& DiskSanitizationInterface::getKey() const noexcept { return key_; }

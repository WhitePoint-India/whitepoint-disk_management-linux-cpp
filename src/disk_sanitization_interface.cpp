
#include <disk_sanitization_interface.hpp>

DiskSanitizationInterface::DiskSanitizationInterface(std::string key, sanitization_grade grade)
    : key_(std::move(key)), grade_(grade) {
}

const std::string& DiskSanitizationInterface::getKey() const noexcept { return key_; }
sanitization_grade DiskSanitizationInterface::grade() const noexcept { return grade_; }

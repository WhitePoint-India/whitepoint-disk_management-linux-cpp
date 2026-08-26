
#ifndef DISK_SANITIZATION_INTERFACE_HPP
#define DISK_SANITIZATION_INTERFACE_HPP

#include <string>
#include <disk.hpp>
#include <sanitization_grade.hpp>
#include <sanitization_callback.hpp>

class DiskSanitizationInterface {
public:
    using Callback = SanitizationCallback;

    [[nodiscard]] const std::string& getKey() const noexcept;

    [[nodiscard]] sanitization_grade grade() const noexcept;

    virtual ~DiskSanitizationInterface() noexcept = default;

    virtual void sanitize(Disk& disk, Callback callback) = 0;

protected:
    explicit DiskSanitizationInterface(std::string key, sanitization_grade grade);

private:
    std::string key_;
    sanitization_grade grade_;
};

#endif // DISK_SANITIZATION_INTERFACE_HPP

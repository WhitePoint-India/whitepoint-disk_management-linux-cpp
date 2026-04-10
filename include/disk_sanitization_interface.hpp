
#ifndef DISK_SANITIZATION_INTERFACE_HPP
#define DISK_SANITIZATION_INTERFACE_HPP

#include <string>
#include <disks.hpp>
#include <sanitization_stage.hpp>

class DiskSanitizationInterface {
private:
    std::string key_;
protected:
    explicit DiskSanitizationInterface(std::string key);
public:
    using Callback = SanitizationCallback;

    [[nodiscard]] const std::string& getKey() const noexcept;

    virtual ~DiskSanitizationInterface() noexcept = default;

    virtual void sanitize(DiskVariant& disk, Callback callback) = 0;
};

#endif // DISK_SANITIZATION_INTERFACE_HPP

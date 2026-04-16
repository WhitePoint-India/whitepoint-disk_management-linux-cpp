
#ifndef SECURE_ERASE_HPP
#define SECURE_ERASE_HPP

#include <disk_sanitization_interface.hpp>
#include <sanitization_method_registry.hpp>

class SecureErase: public DiskSanitizationInterface, private AutoRegisterMethod<SecureErase> {
    public:
        [[nodiscard]] static SecureErase& shared();

        SecureErase(const SecureErase&) = delete;
        SecureErase(SecureErase&&) = delete;
        SecureErase& operator=(const SecureErase&) = delete;
        SecureErase& operator=(SecureErase&&) = delete;

        void sanitize(Disk& disk, Callback callback) override;

    private:
        SecureErase();
};

#endif // SECURE_ERASE_HPP

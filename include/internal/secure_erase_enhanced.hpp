
#ifndef SECURE_ERASE_ENHANCED_HPP
#define SECURE_ERASE_ENHANCED_HPP

#include <disk_sanitization_interface.hpp>
#include <sanitization_method_registry.hpp>

class EnhancedSecureErase: public DiskSanitizationInterface, private AutoRegisterMethod<EnhancedSecureErase> {
    public:
        [[nodiscard]] static EnhancedSecureErase& shared();

        EnhancedSecureErase(const EnhancedSecureErase&) = delete;
        EnhancedSecureErase(EnhancedSecureErase&&) = delete;
        EnhancedSecureErase& operator=(const EnhancedSecureErase&) = delete;
        EnhancedSecureErase& operator=(EnhancedSecureErase&&) = delete;

        void sanitize(Disk& disk, Callback callback) override;

    private:
        EnhancedSecureErase();
};

#endif // SECURE_ERASE_ENHANCED_HPP

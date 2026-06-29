
#ifndef SECURE_ERASE_HPP
#define SECURE_ERASE_HPP

#include <localizable_sanitization_stage.hpp>
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

        // ATA SECURITY ERASE UNIT is a single blocking command with no live
        // progress feedback, so this method reports one coarse stage: 0% before
        // issuing the command and 100% once it returns.
        class Stage: public LocalizableSanitizationStage {
        public:

            const static int totalStagesCount = 1;

            static int indexOf(Stage stage);

            enum Value { ERASE };

            Stage(Value v)
                : LocalizableSanitizationStage(
                      Localization::LocalizedString::key("secure_erase.erase.title"),
                      Localization::LocalizedString::key("secure_erase.erase.desc")),
                  value_(v) {}

            operator Value() const { return value_; }

            explicit operator bool() const = delete;

        private:
            Value value_;
        };

    private:
        SecureErase();
};

#endif // SECURE_ERASE_HPP

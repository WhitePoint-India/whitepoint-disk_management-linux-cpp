
#ifndef NIST_CLEAR_HPP
#define NIST_CLEAR_HPP

#include <localizable_sanitization_stage.hpp>
#include <disk_sanitization_interface.hpp>
#include <sanitization_method_registry.hpp>

class NISTClear: public DiskSanitizationInterface, private AutoRegisterMethod<NISTClear> {
    public:

        [[nodiscard]] static NISTClear& shared();

        NISTClear(const NISTClear&) = delete;
        NISTClear(NISTClear&&) = delete;
        NISTClear& operator=(const NISTClear&) = delete;
        NISTClear& operator=(NISTClear&&) = delete;

        void sanitize(Disk& disk, Callback callback) override;

        class Stage: public LocalizableSanitizationStage {
        public:

            const static int totalStagesCount = 3;

            static int indexOf(Stage stage);

            enum Value { PASS_1, PASS_2, PASS_3 };

            Stage(Value v) : value_(v) {}

            operator Value() const { return value_; }

            explicit operator bool() const = delete;

            std::string title() const override;
            std::string description() const override;
            std::string localizedTitle() const override;
            std::string localizedDescription() const override;

        private:
            Value value_;
        };

    private:
        NISTClear();
};

#endif // NIST_CLEAR_HPP


#ifndef NIST_CLEAR_HPP
#define NIST_CLEAR_HPP

#include <sanitization_stage.hpp>
#include <ata_disk_sanitization_interface.hpp>
#include <nvme_disk_sanitization_interface.hpp>

class NISTClear: public NVMeDiskSanitizationInterface, public ATADiskSanitizationInterface {
    public:

        [[nodiscard]] static NISTClear& shared();

        NISTClear(const NISTClear&) = delete;
        NISTClear(NISTClear&&) = delete;
        NISTClear& operator=(const NISTClear&) = delete;
        NISTClear& operator=(NISTClear&&) = delete;

        void sanitize(DiskVariant& disk, Callback callback) override;

        class Stage: public SanitizationStage {
        public:
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
        void deleteDisk(ATADisk& disk, Callback callback) override;
        void deleteDisk(NVMeDisk& disk, Callback callback) override;
};

#endif // NIST_CLEAR_HPP
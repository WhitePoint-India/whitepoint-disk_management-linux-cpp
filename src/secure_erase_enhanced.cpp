
#include <stdexcept>
#include <nvme_sanitizable.hpp>
#include <ata_secure_erasable.hpp>
#include <secure_erase_enhanced.hpp>

EnhancedSecureErase::EnhancedSecureErase() : DiskSanitizationInterface("ENHANCED_SECURE_ERASE", sanitization_grade::enhanced_secure_erase), AutoRegisterMethod(*this) {

}

EnhancedSecureErase& EnhancedSecureErase::shared() {
    static EnhancedSecureErase instance;
    return instance;
}

void EnhancedSecureErase::sanitize(Disk& disk, Callback callback) {
    if (auto* ata = dynamic_cast<ATASecureErasable*>(&disk)) {
        ata->secureEraseEnhanced([callback](const double fractionCompleted) {
            Stage stage = Stage::ERASE;
            callback(SanitizationProgress(stage, Stage::indexOf(stage), Stage::totalStagesCount, fractionCompleted));
        });
    } else if (auto* nvme = dynamic_cast<NVMeSanitizable*>(&disk)) {
        nvme->sanitize(NVMeSanitizable::Action::CryptoErase, [callback](const double fractionCompleted) {
            Stage stage = Stage::ERASE;
            callback(SanitizationProgress(stage, Stage::indexOf(stage), Stage::totalStagesCount, fractionCompleted));
        });
    } else {
        throw std::invalid_argument("Enhanced Secure Erase is not supported for this disk type");
    }
}

int EnhancedSecureErase::Stage::indexOf(Stage stage) {
    return static_cast<int>(stage.value_);
}

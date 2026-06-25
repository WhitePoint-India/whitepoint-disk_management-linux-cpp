
#include <stdexcept>
#include <nvme_sanitizable.hpp>
#include <ata_secure_erasable.hpp>
#include <secure_erase_enhanced.hpp>

EnhancedSecureErase::EnhancedSecureErase() : DiskSanitizationInterface("ENHANCED_SECURE_ERASE"), AutoRegisterMethod(*this) {

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

std::string EnhancedSecureErase::Stage::title() const {
    switch (value_) {
        case ERASE: return "Enhanced Secure Erase";
        default: return "Unknown Stage";
    }
}

std::string EnhancedSecureErase::Stage::description() const {
    switch (value_) {
        case ERASE: return "Issuing ATA SECURITY ERASE UNIT command in enhanced mode.";
        default: return "Unknown stage for enhanced secure erase.";
    }
}

std::string EnhancedSecureErase::Stage::localizedTitle() const {
    return "Enhanced Secure Erase Stage";
}

std::string EnhancedSecureErase::Stage::localizedDescription() const {
    return "A stage for ATA enhanced secure erase.";
}

int EnhancedSecureErase::Stage::indexOf(Stage stage) {
    return static_cast<int>(stage.value_);
}

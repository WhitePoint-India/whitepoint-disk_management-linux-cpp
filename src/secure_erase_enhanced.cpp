
#include <secure_erase_enhanced.hpp>
#include <ata_secure_erasable.hpp>
#include <nvme_sanitizable.hpp>

#include <stdexcept>

EnhancedSecureErase::EnhancedSecureErase() : DiskSanitizationInterface("SECURE_ERASE_ENHANCED"), AutoRegisterMethod(*this) {

}

EnhancedSecureErase& EnhancedSecureErase::shared() {
    static EnhancedSecureErase instance;
    return instance;
}

void EnhancedSecureErase::sanitize(Disk& disk, Callback callback) {
    Stage stage = Stage::ERASE;
    callback(SanitizationProgress(stage, Stage::indexOf(stage), Stage::totalStagesCount, 0.0));

    if (auto* ata = dynamic_cast<ATASecureErasable*>(&disk)) {
        ata->secureEraseEnhanced();
    } else if (auto* nvme = dynamic_cast<NVMeSanitizable*>(&disk)) {
        nvme->nvmeSanitize(0);
    } else {
        throw std::invalid_argument("Enhanced Secure Erase is not supported for this disk type");
    }

    callback(SanitizationProgress(stage, Stage::indexOf(stage), Stage::totalStagesCount, 1.0));
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

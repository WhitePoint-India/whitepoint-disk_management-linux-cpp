
#include <secure_erase.hpp>
#include <ata_secure_erasable.hpp>
#include <nvme_sanitizable.hpp>

#include <stdexcept>

SecureErase::SecureErase() : DiskSanitizationInterface("SECURE_ERASE"), AutoRegisterMethod(*this) {

}

SecureErase& SecureErase::shared() {
    static SecureErase instance;
    return instance;
}

void SecureErase::sanitize(Disk& disk, Callback callback) {
    Stage stage = Stage::ERASE;
    callback(SanitizationProgress(stage, Stage::indexOf(stage), Stage::totalStagesCount, 0.0));

    if (auto* ata = dynamic_cast<ATASecureErasable*>(&disk)) {
        ata->secureErase();
    } else if (auto* nvme = dynamic_cast<NVMeSanitizable*>(&disk)) {
        nvme->nvmeSanitize(0);
    } else {
        throw std::invalid_argument("Secure Erase is not supported for this disk type");
    }

    callback(SanitizationProgress(stage, Stage::indexOf(stage), Stage::totalStagesCount, 1.0));
}

std::string SecureErase::Stage::title() const {
    switch (value_) {
        case ERASE: return "Secure Erase";
        default: return "Unknown Stage";
    }
}

std::string SecureErase::Stage::description() const {
    switch (value_) {
        case ERASE: return "Issuing ATA SECURITY ERASE UNIT command.";
        default: return "Unknown stage for secure erase.";
    }
}

std::string SecureErase::Stage::localizedTitle() const {
    return "Secure Erase Stage";
}

std::string SecureErase::Stage::localizedDescription() const {
    return "A stage for ATA secure erase.";
}

int SecureErase::Stage::indexOf(Stage stage) {
    return static_cast<int>(stage.value_);
}

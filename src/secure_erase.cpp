
#include <stdexcept>
#include <secure_erase.hpp>
#include <ata_secure_erasable.hpp>
#include <nvme_sanitizable.hpp>


SecureErase::SecureErase() : DiskSanitizationInterface("SECURE_ERASE"), AutoRegisterMethod(*this) {

}

SecureErase& SecureErase::shared() {
    static SecureErase instance;
    return instance;
}

void SecureErase::sanitize(Disk& disk, Callback callback) {
    if (auto* ata = dynamic_cast<ATASecureErasable*>(&disk)) {
        ata->secureErase([callback](const double fractionCompleted) {
            Stage stage = Stage::ERASE;
            callback(SanitizationProgress(stage, Stage::indexOf(stage), Stage::totalStagesCount, fractionCompleted));
        });
    } else if (auto* nvme = dynamic_cast<NVMeSanitizable*>(&disk)) {
        nvme->sanitize(NVMeSanitizable::Action::BlockErase, [callback](const double fractionCompleted) {
            Stage stage = Stage::ERASE;
            callback(SanitizationProgress(stage, Stage::indexOf(stage), Stage::totalStagesCount, fractionCompleted));
        });
    } else {
        throw std::invalid_argument("Secure Erase is not supported for this disk type");
    }
}

int SecureErase::Stage::indexOf(Stage stage) {
    return static_cast<int>(stage.value_);
}

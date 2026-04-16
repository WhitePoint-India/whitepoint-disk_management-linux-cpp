
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
    if (auto* ata = dynamic_cast<ATASecureErasable*>(&disk)) {
        ata->secureEraseUnit(false);
    } else if (auto* nvme = dynamic_cast<NVMeSanitizable*>(&disk)) {
        nvme->nvmeSanitize(0);
    } else {
        throw std::invalid_argument("Secure Erase is not supported for this disk type");
    }
    // TODO: Implement progress reporting via callback
}

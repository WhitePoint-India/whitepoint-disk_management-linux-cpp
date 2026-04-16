
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
    if (auto* ata = dynamic_cast<ATASecureErasable*>(&disk)) {
        ata->secureEraseUnit(true);
    } else if (auto* nvme = dynamic_cast<NVMeSanitizable*>(&disk)) {
        nvme->nvmeSanitize(0);
    } else {
        throw std::invalid_argument("Enhanced Secure Erase is not supported for this disk type");
    }
    // TODO: Implement progress reporting via callback
}

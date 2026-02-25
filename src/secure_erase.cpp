
#include <secure_erase.hpp>

namespace DiskManagement {

SecureErase::SecureErase() : SingletonMethod("SECURE_ERASE") {

}

const std::string& SecureErase::getTitle() const {
    static const std::string title = "SECURE ERASE";
    return title;
}

const std::string& SecureErase::getDescription() const {
    static const std::string description = "ATA Secure Erase command. Uses the drive's built-in firmware to overwrite all data including reallocated sectors.";
    return description;
}

const std::vector<Stage>& SecureErase::getStages() const {
    static const std::vector<Stage> stages = {
        {"secure_erase", "Secure erase", 0, 1}
    };
    return stages;
}

} // namespace DiskManagement

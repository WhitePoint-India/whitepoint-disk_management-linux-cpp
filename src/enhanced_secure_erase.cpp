
#include <enhanced_secure_erase.hpp>

namespace DiskManagement {

EnhancedSecureErase::EnhancedSecureErase() : SingletonMethod("ENHANCED_SECURE_ERASE") {

}

const std::string& EnhancedSecureErase::getTitle() const {
    static const std::string title = "ENHANCED SECURE ERASE";
    return title;
}

const std::string& EnhancedSecureErase::getDescription() const {
    static const std::string description = "ATA Enhanced Secure Erase command. Uses vendor-specific patterns to overwrite all data including reallocated and reserved sectors.";
    return description;
}

const std::vector<Stage>& EnhancedSecureErase::getStages() const {
    static const std::vector<Stage> stages = {
        {"enhanced_secure_erase", "Enhanced secure erase", 0, 1}
    };
    return stages;
}

} // namespace DiskManagement

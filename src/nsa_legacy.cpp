
#include <nsa_legacy.hpp>

namespace DiskManagement {

NSALegacy::NSALegacy() : SingletonMethod("NSA_LEGACY") {

}

const std::string& NSALegacy::getTitle() const {
    static const std::string title = "NSA LEGACY";
    return title;
}

const std::string& NSALegacy::getDescription() const {
    static const std::string description = "NSA legacy sanitization method using alternating random and zero fill passes for magnetic media.";
    return description;
}

const std::vector<Stage>& NSALegacy::getStages() const {
    static const std::vector<Stage> stages = {
        {"random_pass", "Writing random data", 0, 2},
        {"zero_pass", "Writing zeros", 1, 2}
    };
    return stages;
}

} // namespace DiskManagement

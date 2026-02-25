
#include <nist_800_88_purge.hpp>

namespace DiskManagement {

NIST80088Purge::NIST80088Purge() : SingletonMethod("NIST_800_88_PURGE") {

}

const std::string& NIST80088Purge::getTitle() const {
    static const std::string title = "NIST 800-88 PURGE";
    return title;
}

const std::string& NIST80088Purge::getDescription() const {
    static const std::string description = "NIST 800-88 Purge level sanitization. Applies physical or logical techniques that render data recovery infeasible using state-of-the-art laboratory techniques.";
    return description;
}

const std::vector<Stage>& NIST80088Purge::getStages() const {
    static const std::vector<Stage> stages = {
        {"hpa_detection", "HPA detection and removal", 0, 4},
        {"dco_detection", "DCO detection and removal", 1, 4},
        {"secure_erase", "Secure erase", 2, 4},
        {"verification", "Verification", 3, 4}
    };
    return stages;
}

} // namespace DiskManagement


#include <nsa_modern.hpp>

namespace DiskManagement {

NSAModern::NSAModern() : SingletonMethod("NSA_MODERN") {

}

const std::string& NSAModern::getTitle() const {
    static const std::string title = "NSA MODERN";
    return title;
}

const std::string& NSAModern::getDescription() const {
    static const std::string description = "NSA modern sanitization method using degaussing and physical destruction for classified media.";
    return description;
}

const std::vector<Stage>& NSAModern::getStages() const {
    static const std::vector<Stage> stages = {
        {"degauss", "Degaussing", 0, 1}
    };
    return stages;
}

} // namespace DiskManagement

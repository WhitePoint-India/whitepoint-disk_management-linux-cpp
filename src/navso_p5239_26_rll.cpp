
#include <navso_p5239_26_rll.hpp>

namespace DiskManagement {

NAVSOP523926RLL::NAVSOP523926RLL() : SingletonMethod("NAVSO_P5239_26_RLL") {

}

const std::string& NAVSOP523926RLL::getTitle() const {
    static const std::string title = "NAVSO P-5239-26 (RLL)";
    return title;
}

const std::string& NAVSOP523926RLL::getDescription() const {
    static const std::string description = "U.S. Navy NAVSO P-5239-26 3-pass overwrite method for RLL (Run Length Limited) encoded disks.";
    return description;
}

const std::vector<Stage>& NAVSOP523926RLL::getStages() const {
    static const std::vector<Stage> stages = {
        {"pass_1", "Pass 1 - Writing 0x01", 0, 3},
        {"pass_2", "Pass 2 - Writing 0x27FFFFFF", 1, 3},
        {"pass_3", "Pass 3 - Writing random data", 2, 3}
    };
    return stages;
}

} // namespace DiskManagement

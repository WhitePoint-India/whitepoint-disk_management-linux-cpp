
#include <afssi_5020.hpp>

namespace DiskManagement {

AFSSI5020::AFSSI5020() : SingletonMethod("AFSSI_5020") {

}

const std::string& AFSSI5020::getTitle() const {
    static const std::string title = "AFSSI-5020";
    return title;
}

const std::string& AFSSI5020::getDescription() const {
    static const std::string description = "U.S. Air Force System Security Instruction 5020 3-pass overwrite method for magnetic media sanitization.";
    return description;
}

const std::vector<Stage>& AFSSI5020::getStages() const {
    static const std::vector<Stage> stages = {
        {"pass_1", "Pass 1 - Writing 0x00", 0, 3},
        {"pass_2", "Pass 2 - Writing 0xFF", 1, 3},
        {"pass_3", "Pass 3 - Writing random data", 2, 3}
    };
    return stages;
}

} // namespace DiskManagement

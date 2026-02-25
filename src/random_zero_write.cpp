
#include <random_zero_write.hpp>

namespace DiskManagement {

RandomZeroWrite::RandomZeroWrite() : SingletonMethod("RANDOM_ZERO_FILL") {

}

const std::string& RandomZeroWrite::getTitle() const {
    static const std::string title = "RANDOM ZERO WRITE";
    return title;
}

const std::string& RandomZeroWrite::getDescription() const {
    static const std::string description = "Writes random data followed by a zero fill pass to all bytes of the disk.";
    return description;
}

const std::vector<Stage>& RandomZeroWrite::getStages() const {
    static const std::vector<Stage> stages = {
        {"random_pass", "Writing random data", 0, 2},
        {"zero_pass", "Writing zeros", 1, 2}
    };
    return stages;
}

} // namespace DiskManagement

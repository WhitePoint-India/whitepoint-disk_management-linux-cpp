
#include <random_write.hpp>

namespace DiskManagement {

RandomWrite::RandomWrite() : SingletonMethod("RANDOM_FILL") {

}

const std::string& RandomWrite::getTitle() const {
    static const std::string title = "RANDOM WRITE";
    return title;
}

const std::string& RandomWrite::getDescription() const {
    static const std::string description = "Writes cryptographically random data to all bytes of the disk.";
    return description;
}

const std::vector<Stage>& RandomWrite::getStages() const {
    static const std::vector<Stage> stages = {
        {"random_pass", "Writing random data", 0, 1}
    };
    return stages;
}

} // namespace DiskManagement

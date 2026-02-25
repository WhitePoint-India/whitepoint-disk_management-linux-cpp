
#include <gutmann.hpp>

namespace DiskManagement {

Gutmann::Gutmann() : SingletonMethod("GUTMANN") {

}

const std::string& Gutmann::getTitle() const {
    static const std::string title = "Peter Gutmann";
    return title;
}

const std::string& Gutmann::getDescription() const {
    static const std::string description = "Peter Gutmann's 35-pass overwrite method designed for older magnetic media encoding schemes.";
    return description;
}

const std::vector<Stage>& Gutmann::getStages() const {
    static const std::vector<Stage> stages = {
        {"pass_1",  "Pass 1 - Random data",   0, 35},
        {"pass_2",  "Pass 2 - Random data",   1, 35},
        {"pass_3",  "Pass 3 - Random data",   2, 35},
        {"pass_4",  "Pass 4 - Random data",   3, 35},
        {"pass_5",  "Pass 5 - Fixed pattern",  4, 35},
        {"pass_6",  "Pass 6 - Fixed pattern",  5, 35},
        {"pass_7",  "Pass 7 - Fixed pattern",  6, 35},
        {"pass_8",  "Pass 8 - Fixed pattern",  7, 35},
        {"pass_9",  "Pass 9 - Fixed pattern",  8, 35},
        {"pass_10", "Pass 10 - Fixed pattern",  9, 35},
        {"pass_11", "Pass 11 - Fixed pattern", 10, 35},
        {"pass_12", "Pass 12 - Fixed pattern", 11, 35},
        {"pass_13", "Pass 13 - Fixed pattern", 12, 35},
        {"pass_14", "Pass 14 - Fixed pattern", 13, 35},
        {"pass_15", "Pass 15 - Fixed pattern", 14, 35},
        {"pass_16", "Pass 16 - Fixed pattern", 15, 35},
        {"pass_17", "Pass 17 - Fixed pattern", 16, 35},
        {"pass_18", "Pass 18 - Fixed pattern", 17, 35},
        {"pass_19", "Pass 19 - Fixed pattern", 18, 35},
        {"pass_20", "Pass 20 - Fixed pattern", 19, 35},
        {"pass_21", "Pass 21 - Fixed pattern", 20, 35},
        {"pass_22", "Pass 22 - Fixed pattern", 21, 35},
        {"pass_23", "Pass 23 - Fixed pattern", 22, 35},
        {"pass_24", "Pass 24 - Fixed pattern", 23, 35},
        {"pass_25", "Pass 25 - Fixed pattern", 24, 35},
        {"pass_26", "Pass 26 - Fixed pattern", 25, 35},
        {"pass_27", "Pass 27 - Fixed pattern", 26, 35},
        {"pass_28", "Pass 28 - Fixed pattern", 27, 35},
        {"pass_29", "Pass 29 - Fixed pattern", 28, 35},
        {"pass_30", "Pass 30 - Fixed pattern", 29, 35},
        {"pass_31", "Pass 31 - Fixed pattern", 30, 35},
        {"pass_32", "Pass 32 - Random data",  31, 35},
        {"pass_33", "Pass 33 - Random data",  32, 35},
        {"pass_34", "Pass 34 - Random data",  33, 35},
        {"pass_35", "Pass 35 - Random data",  34, 35},
    };
    return stages;
}

} // namespace DiskManagement

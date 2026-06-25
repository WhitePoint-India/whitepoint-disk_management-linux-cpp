
#include <gutmann.hpp>

#include <string>

std::vector<OverwriteMethod::Pass> Gutmann::buildPasses() {
    // Gutmann passes 5-31: fixed 3-byte patterns (passes 1-4 and 32-35 are random).
    static const std::vector<std::vector<unsigned char>> fixed = {
        {0x55, 0x55, 0x55}, {0xAA, 0xAA, 0xAA}, {0x92, 0x49, 0x24}, {0x49, 0x24, 0x92},
        {0x24, 0x92, 0x49}, {0x00, 0x00, 0x00}, {0x11, 0x11, 0x11}, {0x22, 0x22, 0x22},
        {0x33, 0x33, 0x33}, {0x44, 0x44, 0x44}, {0x55, 0x55, 0x55}, {0x66, 0x66, 0x66},
        {0x77, 0x77, 0x77}, {0x88, 0x88, 0x88}, {0x99, 0x99, 0x99}, {0xAA, 0xAA, 0xAA},
        {0xBB, 0xBB, 0xBB}, {0xCC, 0xCC, 0xCC}, {0xDD, 0xDD, 0xDD}, {0xEE, 0xEE, 0xEE},
        {0xFF, 0xFF, 0xFF}, {0x92, 0x49, 0x24}, {0x49, 0x24, 0x92}, {0x24, 0x92, 0x49},
        {0x6D, 0xB6, 0xDB}, {0xB6, 0xDB, 0x6D}, {0xDB, 0x6D, 0xB6},
    };

    std::vector<Pass> passes;
    passes.reserve(35);

    int passNumber = 1;
    for (int i = 0; i < 4; ++i) {  // passes 1-4: random
        passes.push_back({"Pass " + std::to_string(passNumber++) + " - Random data",
                          Pass::Kind::Write, std::nullopt});
    }
    for (const auto& pattern : fixed) {  // passes 5-31: fixed patterns
        passes.push_back({"Pass " + std::to_string(passNumber++) + " - Fixed pattern",
                          Pass::Kind::Write, pattern});
    }
    for (int i = 0; i < 4; ++i) {  // passes 32-35: random
        passes.push_back({"Pass " + std::to_string(passNumber++) + " - Random data",
                          Pass::Kind::Write, std::nullopt});
    }
    return passes;
}

Gutmann::Gutmann() : OverwriteMethod("GUTMANN", buildPasses()), AutoRegisterMethod(*this) {
}

Gutmann& Gutmann::shared() {
    static Gutmann instance;
    return instance;
}

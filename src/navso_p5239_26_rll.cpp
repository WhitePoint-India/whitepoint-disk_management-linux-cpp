
#include <navso_p5239_26_rll.hpp>

NAVSOP523926RLL::NAVSOP523926RLL() : OverwriteMethod("NAVSO_P5239_26_RLL", {
    {"Pass 1 - Writing 0x01", Pass::Kind::Write, std::vector<unsigned char>{0x01}},
    {"Pass 2 - Writing 0x27FFFFFF", Pass::Kind::Write, std::vector<unsigned char>{0x27, 0xFF, 0xFF, 0xFF}},
    {"Pass 3 - Writing random data", Pass::Kind::Write, std::nullopt},
    {"Pass 4 - Verification", Pass::Kind::Verify, std::nullopt},
}), AutoRegisterMethod(*this) {
}

NAVSOP523926RLL& NAVSOP523926RLL::shared() {
    static NAVSOP523926RLL instance;
    return instance;
}

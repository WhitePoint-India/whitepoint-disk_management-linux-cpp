
#include <navso_p5239_26_mfm.hpp>

NAVSOP523926MFM::NAVSOP523926MFM() : OverwriteMethod("NAVSO_P5239_26_MFM", {
    {"Pass 1 - Writing 0x01", Pass::Kind::Write, std::vector<unsigned char>{0x01}},
    {"Pass 2 - Writing 0x7FFFFFFF", Pass::Kind::Write, std::vector<unsigned char>{0x7F, 0xFF, 0xFF, 0xFF}},
    {"Pass 3 - Writing random data", Pass::Kind::Write, std::nullopt},
    {"Pass 4 - Verification", Pass::Kind::Verify, std::nullopt},
}), AutoRegisterMethod(*this) {
}

NAVSOP523926MFM& NAVSOP523926MFM::shared() {
    static NAVSOP523926MFM instance;
    return instance;
}

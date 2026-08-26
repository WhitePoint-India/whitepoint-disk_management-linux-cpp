
#include <navso_p5239_26_mfm.hpp>

NAVSOP523926MFM::NAVSOP523926MFM() : OverwriteMethod("NAVSO_P5239_26_MFM", sanitization_grade::navso_p5239_26_mfm,
    Pass(Pattern::sequence(0x01)),
    Pass(Pattern::sequence(0x7F, 0xFF, 0xFF, 0xFF)),
    Pass(Pattern::random())
), AutoRegisterMethod(*this) {
}

NAVSOP523926MFM& NAVSOP523926MFM::shared() {
    static NAVSOP523926MFM instance;
    return instance;
}


#include <navso_p5239_26_rll.hpp>

NAVSOP523926RLL::NAVSOP523926RLL() : OverwriteMethod("NAVSO_P5239_26_RLL", sanitization_grade::navso_p5239_26_rll,
    Pass(Pattern::sequence(0x01)),
    Pass(Pattern::sequence(0x27, 0xFF, 0xFF, 0xFF)),
    Pass(Pattern::random())
), AutoRegisterMethod(*this) {
}

NAVSOP523926RLL& NAVSOP523926RLL::shared() {
    static NAVSOP523926RLL instance;
    return instance;
}

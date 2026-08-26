
#include <bit_toggle.hpp>

BitToggle::BitToggle() : OverwriteMethod("NSA_LEGACY", sanitization_grade::nsa_legacy,
    Pass(Pattern::repeat(RepeatingByte::ZERO)),
    Pass(Pattern::repeat(RepeatingByte::ONE)),
    Pass(Pattern::repeat(RepeatingByte::ZERO)),
    Pass(Pattern::repeat(RepeatingByte::ONE))
), AutoRegisterMethod(*this) {
}

BitToggle& BitToggle::shared() {
    static BitToggle instance;
    return instance;
}


#include <nist_800.hpp>

NIST800::NIST800() : SecureEraseOverwriteMethod("NIST_800", sanitization_grade::nist_800_88,
    Pass(Pattern::repeat(RepeatingByte::ZERO))
), AutoRegisterMethod(*this) {
}

NIST800& NIST800::shared() {
    static NIST800 instance;
    return instance;
}

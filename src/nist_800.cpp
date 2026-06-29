
#include <nist_800.hpp>

NIST800::NIST800() : SecureEraseOverwriteMethod("NIST_800",
    Pass(Pattern::repeat(RepeatingByte::ZERO))
), AutoRegisterMethod(*this) {
}

NIST800& NIST800::shared() {
    static NIST800 instance;
    return instance;
}

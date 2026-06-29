
#include <nist_800_adv.hpp>

NIST800Adv::NIST800Adv() : SecureEraseOverwriteMethod("NIST_800_ADV",
    Pass(Pattern::random()),
    Pass(Pattern::repeat(RepeatingByte::ZERO))
), AutoRegisterMethod(*this) {
}

NIST800Adv& NIST800Adv::shared() {
    static NIST800Adv instance;
    return instance;
}

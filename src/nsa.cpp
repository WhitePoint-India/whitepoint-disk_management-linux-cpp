
#include <nsa.hpp>

NSA::NSA() : OverwriteMethod("NSA_MODERN", sanitization_grade::nsa_modern,
    Pass(Pattern::random()),
    Pass(Pattern::random()),
    Pass(Pattern::repeat(RepeatingByte::ZERO))
), AutoRegisterMethod(*this) {
}

NSA& NSA::shared() {
    static NSA instance;
    return instance;
}

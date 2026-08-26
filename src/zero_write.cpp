
#include <zero_write.hpp>

ZeroWrite::ZeroWrite() : OverwriteMethod("ZERO_FILL", sanitization_grade::zero_write,
    Pass(Pattern::repeat(RepeatingByte::ZERO))
), AutoRegisterMethod(*this) {
}

ZeroWrite& ZeroWrite::shared() {
    static ZeroWrite instance;
    return instance;
}

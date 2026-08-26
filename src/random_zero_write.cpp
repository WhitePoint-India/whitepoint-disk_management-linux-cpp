
#include <random_zero_write.hpp>

RandomZeroWrite::RandomZeroWrite() : OverwriteMethod("RANDOM_ZERO_FILL", sanitization_grade::random_zero_write,
    Pass(Pattern::random()),
    Pass(Pattern::repeat(RepeatingByte::ZERO))
), AutoRegisterMethod(*this) {
}

RandomZeroWrite& RandomZeroWrite::shared() {
    static RandomZeroWrite instance;
    return instance;
}

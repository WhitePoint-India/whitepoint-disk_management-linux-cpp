
#include <zero_write.hpp>

ZeroWrite::ZeroWrite() : OverwriteMethod("ZERO_FILL",
    Pass(Pattern::repeat(RepeatingByte::ZERO))
), AutoRegisterMethod(*this) {
}

ZeroWrite& ZeroWrite::shared() {
    static ZeroWrite instance;
    return instance;
}

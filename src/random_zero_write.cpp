
#include <random_zero_write.hpp>

RandomZeroWrite::RandomZeroWrite() : OverwriteMethod("RANDOM_ZERO_FILL", {
    {"Pass 1 - Writing random data", Pass::Kind::Write, std::nullopt},
    {"Pass 2 - Writing zeros", Pass::Kind::Write, std::vector<unsigned char>{0x00}},
}), AutoRegisterMethod(*this) {
}

RandomZeroWrite& RandomZeroWrite::shared() {
    static RandomZeroWrite instance;
    return instance;
}

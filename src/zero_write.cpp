
#include <zero_write.hpp>

ZeroWrite::ZeroWrite() : OverwriteMethod("ZERO_FILL", {
    {"Writing zeros", Pass::Kind::Write, std::vector<unsigned char>{0x00}},
}), AutoRegisterMethod(*this) {
}

ZeroWrite& ZeroWrite::shared() {
    static ZeroWrite instance;
    return instance;
}

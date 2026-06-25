
#include <nsa.hpp>

NSA::NSA() : OverwriteMethod("NSA_MODERN", {
    {"Pass 1 - Writing random data", Pass::Kind::Write, std::nullopt},
    {"Pass 2 - Writing random data", Pass::Kind::Write, std::nullopt},
    {"Pass 3 - Writing zeros", Pass::Kind::Write, std::vector<unsigned char>{0x00}},
}), AutoRegisterMethod(*this) {
}

NSA& NSA::shared() {
    static NSA instance;
    return instance;
}


#include <dod_5220_28_m.hpp>

DoD522028M::DoD522028M() : OverwriteMethod("DOD_5220_28_M", {
    {"Pass 1 - Writing 0xFF", Pass::Kind::Write, std::vector<unsigned char>{0xFF}},
    {"Pass 2 - Writing 0x00", Pass::Kind::Write, std::vector<unsigned char>{0x00}},
    {"Pass 3 - Writing random data", Pass::Kind::Write, std::nullopt},
}), AutoRegisterMethod(*this) {
}

DoD522028M& DoD522028M::shared() {
    static DoD522028M instance;
    return instance;
}


#include <dod_5220_22_m.hpp>

DoD522022M::DoD522022M() : OverwriteMethod("DOD_5220_22_M", {
    {"Pass 1 - Writing 0x00", Pass::Kind::Write, std::vector<unsigned char>{0x00}},
    {"Pass 2 - Writing 0xFF", Pass::Kind::Write, std::vector<unsigned char>{0xFF}},
    {"Pass 3 - Writing random data", Pass::Kind::Write, std::nullopt},
    {"Pass 4 - Verification", Pass::Kind::Verify, std::nullopt},
}), AutoRegisterMethod(*this) {
}

DoD522022M& DoD522022M::shared() {
    static DoD522022M instance;
    return instance;
}

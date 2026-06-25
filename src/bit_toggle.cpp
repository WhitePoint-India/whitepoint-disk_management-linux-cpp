
#include <bit_toggle.hpp>

BitToggle::BitToggle() : OverwriteMethod("NSA_LEGACY", {
    {"Pass 1 - Writing 0x00", Pass::Kind::Write, std::vector<unsigned char>{0x00}},
    {"Pass 2 - Writing 0xFF", Pass::Kind::Write, std::vector<unsigned char>{0xFF}},
    {"Pass 3 - Writing 0x00", Pass::Kind::Write, std::vector<unsigned char>{0x00}},
    {"Pass 4 - Writing 0xFF", Pass::Kind::Write, std::vector<unsigned char>{0xFF}},
}), AutoRegisterMethod(*this) {
}

BitToggle& BitToggle::shared() {
    static BitToggle instance;
    return instance;
}


#include <bsi_vsitr.hpp>

BSIVSITR::BSIVSITR() : OverwriteMethod("BSI_VSITR", {
    {"Pass 1 - Writing 0x00", Pass::Kind::Write, std::vector<unsigned char>{0x00}},
    {"Pass 2 - Writing 0xFF", Pass::Kind::Write, std::vector<unsigned char>{0xFF}},
    {"Pass 3 - Writing 0x00", Pass::Kind::Write, std::vector<unsigned char>{0x00}},
    {"Pass 4 - Writing 0xFF", Pass::Kind::Write, std::vector<unsigned char>{0xFF}},
    {"Pass 5 - Writing 0x00", Pass::Kind::Write, std::vector<unsigned char>{0x00}},
    {"Pass 6 - Writing 0xFF", Pass::Kind::Write, std::vector<unsigned char>{0xFF}},
    {"Pass 7 - Writing 0xAA", Pass::Kind::Write, std::vector<unsigned char>{0xAA}},
}), AutoRegisterMethod(*this) {
}

BSIVSITR& BSIVSITR::shared() {
    static BSIVSITR instance;
    return instance;
}

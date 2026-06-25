
#include <afssi_5020.hpp>

AFSSI5020::AFSSI5020() : OverwriteMethod("AFSSI_5020", {
    {"Pass 1 - Writing 0x00", Pass::Kind::Write, std::vector<unsigned char>{0x00}},
    {"Pass 2 - Writing 0xFF", Pass::Kind::Write, std::vector<unsigned char>{0xFF}},
    {"Pass 3 - Writing random data", Pass::Kind::Write, std::nullopt},
    {"Pass 4 - Verification (10% sample)", Pass::Kind::Verify, std::nullopt, 0.10},
}), AutoRegisterMethod(*this) {
}

AFSSI5020& AFSSI5020::shared() {
    static AFSSI5020 instance;
    return instance;
}

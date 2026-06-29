
#include <afssi_5020.hpp>

AFSSI5020::AFSSI5020() : OverwriteMethod("AFSSI_5020",
    Pass(Pattern::repeat(RepeatingByte::ZERO)),
    Pass(Pattern::repeat(RepeatingByte::ONE)),
    Pass(Pattern::random())
), AutoRegisterMethod(*this) {
}

AFSSI5020& AFSSI5020::shared() {
    static AFSSI5020 instance;
    return instance;
}

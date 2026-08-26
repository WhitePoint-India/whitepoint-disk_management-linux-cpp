
#include <bsi_vsitr.hpp>

BSIVSITR::BSIVSITR() : OverwriteMethod("BSI_VSITR", sanitization_grade::bsi_vsitr,
    Pass(Pattern::repeat(RepeatingByte::ZERO)),
    Pass(Pattern::repeat(RepeatingByte::ONE)),
    Pass(Pattern::repeat(RepeatingByte::ZERO)),
    Pass(Pattern::repeat(RepeatingByte::ONE)),
    Pass(Pattern::repeat(RepeatingByte::ZERO)),
    Pass(Pattern::repeat(RepeatingByte::ONE)),
    Pass(Pattern::sequence(0xAA))
), AutoRegisterMethod(*this) {
}

BSIVSITR& BSIVSITR::shared() {
    static BSIVSITR instance;
    return instance;
}

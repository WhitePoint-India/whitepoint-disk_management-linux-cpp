
#include <dod_5220_22_m.hpp>

DoD522022M::DoD522022M() : OverwriteMethod("DOD_5220_22_M",
    Pass(Pattern::repeat(RepeatingByte::ZERO)),
    Pass(Pattern::repeat(RepeatingByte::ONE)),
    Pass(Pattern::random())
), AutoRegisterMethod(*this) {
}

DoD522022M& DoD522022M::shared() {
    static DoD522022M instance;
    return instance;
}

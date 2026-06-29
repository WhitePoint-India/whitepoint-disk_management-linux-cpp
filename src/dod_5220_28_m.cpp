
#include <dod_5220_28_m.hpp>

DoD522028M::DoD522028M() : OverwriteMethod("DOD_5220_28_M",
    Pass(Pattern::repeat(RepeatingByte::ONE)),
    Pass(Pattern::repeat(RepeatingByte::ZERO)),
    Pass(Pattern::random())
), AutoRegisterMethod(*this) {
}

DoD522028M& DoD522028M::shared() {
    static DoD522028M instance;
    return instance;
}

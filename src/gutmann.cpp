
#include <gutmann.hpp>

// Gutmann 35-pass method: passes 1-4 and 32-35 are random; passes 5-31 are the
// 27 fixed patterns below.
Gutmann::Gutmann() : OverwriteMethod("GUTMANN", sanitization_grade::peter_gutmann,
    Pass(Pattern::random()),
    Pass(Pattern::random()),
    Pass(Pattern::random()),
    Pass(Pattern::random()),
    Pass(Pattern::sequence(0x55, 0x55, 0x55)),
    Pass(Pattern::sequence(0xAA, 0xAA, 0xAA)),
    Pass(Pattern::sequence(0x92, 0x49, 0x24)),
    Pass(Pattern::sequence(0x49, 0x24, 0x92)),
    Pass(Pattern::sequence(0x24, 0x92, 0x49)),
    Pass(Pattern::sequence(0x00, 0x00, 0x00)),
    Pass(Pattern::sequence(0x11, 0x11, 0x11)),
    Pass(Pattern::sequence(0x22, 0x22, 0x22)),
    Pass(Pattern::sequence(0x33, 0x33, 0x33)),
    Pass(Pattern::sequence(0x44, 0x44, 0x44)),
    Pass(Pattern::sequence(0x55, 0x55, 0x55)),
    Pass(Pattern::sequence(0x66, 0x66, 0x66)),
    Pass(Pattern::sequence(0x77, 0x77, 0x77)),
    Pass(Pattern::sequence(0x88, 0x88, 0x88)),
    Pass(Pattern::sequence(0x99, 0x99, 0x99)),
    Pass(Pattern::sequence(0xAA, 0xAA, 0xAA)),
    Pass(Pattern::sequence(0xBB, 0xBB, 0xBB)),
    Pass(Pattern::sequence(0xCC, 0xCC, 0xCC)),
    Pass(Pattern::sequence(0xDD, 0xDD, 0xDD)),
    Pass(Pattern::sequence(0xEE, 0xEE, 0xEE)),
    Pass(Pattern::sequence(0xFF, 0xFF, 0xFF)),
    Pass(Pattern::sequence(0x92, 0x49, 0x24)),
    Pass(Pattern::sequence(0x49, 0x24, 0x92)),
    Pass(Pattern::sequence(0x24, 0x92, 0x49)),
    Pass(Pattern::sequence(0x6D, 0xB6, 0xDB)),
    Pass(Pattern::sequence(0xB6, 0xDB, 0x6D)),
    Pass(Pattern::sequence(0xDB, 0x6D, 0xB6)),
    Pass(Pattern::random()),
    Pass(Pattern::random()),
    Pass(Pattern::random()),
    Pass(Pattern::random())
), AutoRegisterMethod(*this) {
}

Gutmann& Gutmann::shared() {
    static Gutmann instance;
    return instance;
}

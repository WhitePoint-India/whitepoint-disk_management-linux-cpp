
#include <random_write.hpp>

RandomWrite::RandomWrite() : OverwriteMethod("RANDOM_FILL",
    Pass(Pattern::random())
), AutoRegisterMethod(*this) {
}

RandomWrite& RandomWrite::shared() {
    static RandomWrite instance;
    return instance;
}

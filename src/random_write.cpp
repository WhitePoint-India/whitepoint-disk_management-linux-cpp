
#include <random_write.hpp>

RandomWrite::RandomWrite() : OverwriteMethod("RANDOM_FILL", sanitization_grade::random_write,
    Pass(Pattern::random())
), AutoRegisterMethod(*this) {
}

RandomWrite& RandomWrite::shared() {
    static RandomWrite instance;
    return instance;
}

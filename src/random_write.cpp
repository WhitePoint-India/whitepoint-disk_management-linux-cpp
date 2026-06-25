
#include <random_write.hpp>

RandomWrite::RandomWrite() : OverwriteMethod("RANDOM_FILL", {
    {"Writing random data", Pass::Kind::Write, std::nullopt},
}), AutoRegisterMethod(*this) {
}

RandomWrite& RandomWrite::shared() {
    static RandomWrite instance;
    return instance;
}

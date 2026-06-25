
#ifndef ZERO_WRITE_HPP
#define ZERO_WRITE_HPP

#include <overwrite_method.hpp>
#include <sanitization_method_registry.hpp>

class ZeroWrite: public OverwriteMethod, private AutoRegisterMethod<ZeroWrite> {
public:
    [[nodiscard]] static ZeroWrite& shared();

    ZeroWrite(const ZeroWrite&) = delete;
    ZeroWrite(ZeroWrite&&) = delete;
    ZeroWrite& operator=(const ZeroWrite&) = delete;
    ZeroWrite& operator=(ZeroWrite&&) = delete;

private:
    ZeroWrite();
};

#endif // ZERO_WRITE_HPP

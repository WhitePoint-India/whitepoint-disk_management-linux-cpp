
#ifndef BSI_VSITR_HPP
#define BSI_VSITR_HPP

#include <overwrite_method.hpp>
#include <sanitization_method_registry.hpp>

class BSIVSITR: public OverwriteMethod, private AutoRegisterMethod<BSIVSITR> {
public:
    [[nodiscard]] static BSIVSITR& shared();

    BSIVSITR(const BSIVSITR&) = delete;
    BSIVSITR(BSIVSITR&&) = delete;
    BSIVSITR& operator=(const BSIVSITR&) = delete;
    BSIVSITR& operator=(BSIVSITR&&) = delete;

private:
    BSIVSITR();
};

#endif // BSI_VSITR_HPP

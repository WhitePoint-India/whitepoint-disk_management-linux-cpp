
#ifndef NIST_800_ADV_HPP
#define NIST_800_ADV_HPP

#include <overwrite_verify_method.hpp>
#include <sanitization_method_registry.hpp>

class NIST800Adv: public OverwriteVerifyMethod, private AutoRegisterMethod<NIST800Adv> {
public:
    [[nodiscard]] static NIST800Adv& shared();

    NIST800Adv(const NIST800Adv&) = delete;
    NIST800Adv(NIST800Adv&&) = delete;
    NIST800Adv& operator=(const NIST800Adv&) = delete;
    NIST800Adv& operator=(NIST800Adv&&) = delete;

private:
    NIST800Adv();
};

#endif // NIST_800_ADV_HPP

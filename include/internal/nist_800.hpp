
#ifndef NIST_800_HPP
#define NIST_800_HPP

#include <overwrite_verify_method.hpp>
#include <sanitization_method_registry.hpp>

class NIST800: public OverwriteVerifyMethod, private AutoRegisterMethod<NIST800> {
public:
    [[nodiscard]] static NIST800& shared();

    NIST800(const NIST800&) = delete;
    NIST800(NIST800&&) = delete;
    NIST800& operator=(const NIST800&) = delete;
    NIST800& operator=(NIST800&&) = delete;

private:
    NIST800();
};

#endif // NIST_800_HPP


#ifndef NAVSO_P5239_26_RLL_HPP
#define NAVSO_P5239_26_RLL_HPP

#include <overwrite_method.hpp>
#include <sanitization_method_registry.hpp>

class NAVSOP523926RLL: public OverwriteMethod, private AutoRegisterMethod<NAVSOP523926RLL> {
public:
    [[nodiscard]] static NAVSOP523926RLL& shared();

    NAVSOP523926RLL(const NAVSOP523926RLL&) = delete;
    NAVSOP523926RLL(NAVSOP523926RLL&&) = delete;
    NAVSOP523926RLL& operator=(const NAVSOP523926RLL&) = delete;
    NAVSOP523926RLL& operator=(NAVSOP523926RLL&&) = delete;

private:
    NAVSOP523926RLL();
};

#endif // NAVSO_P5239_26_RLL_HPP

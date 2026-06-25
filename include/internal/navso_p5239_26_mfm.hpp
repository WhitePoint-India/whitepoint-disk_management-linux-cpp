
#ifndef NAVSO_P5239_26_MFM_HPP
#define NAVSO_P5239_26_MFM_HPP

#include <overwrite_method.hpp>
#include <sanitization_method_registry.hpp>

class NAVSOP523926MFM: public OverwriteMethod, private AutoRegisterMethod<NAVSOP523926MFM> {
public:
    [[nodiscard]] static NAVSOP523926MFM& shared();

    NAVSOP523926MFM(const NAVSOP523926MFM&) = delete;
    NAVSOP523926MFM(NAVSOP523926MFM&&) = delete;
    NAVSOP523926MFM& operator=(const NAVSOP523926MFM&) = delete;
    NAVSOP523926MFM& operator=(NAVSOP523926MFM&&) = delete;

private:
    NAVSOP523926MFM();
};

#endif // NAVSO_P5239_26_MFM_HPP

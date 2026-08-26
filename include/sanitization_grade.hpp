#ifndef SANITIZATION_GRADE_HPP
#define SANITIZATION_GRADE_HPP

#include <cstdint>

enum class sanitization_grade : std::uint8_t {
  zero_write = 1,
  random_write,
  nist_800_88,
  random_zero_write,
  nist_800_88_advanced,
  nsa_modern,
  dod_5220_28_m,
  afssi_5020,
  dod_5220_22_m,
  navso_p5239_26_mfm,
  navso_p5239_26_rll,
  nsa_legacy,
  bsi_vsitr,
  peter_gutmann,
  secure_erase,
  enhanced_secure_erase
}; // enum class sanitization_grade

#endif // SANITIZATION_GRADE_HPP
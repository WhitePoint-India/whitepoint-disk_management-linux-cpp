
#ifndef BIT_TOGGLE_HPP
#define BIT_TOGGLE_HPP

#include <overwrite_method.hpp>
#include <sanitization_method_registry.hpp>

class BitToggle: public OverwriteMethod, private AutoRegisterMethod<BitToggle> {
public:
    [[nodiscard]] static BitToggle& shared();

    BitToggle(const BitToggle&) = delete;
    BitToggle(BitToggle&&) = delete;
    BitToggle& operator=(const BitToggle&) = delete;
    BitToggle& operator=(BitToggle&&) = delete;

private:
    BitToggle();
};

#endif // BIT_TOGGLE_HPP

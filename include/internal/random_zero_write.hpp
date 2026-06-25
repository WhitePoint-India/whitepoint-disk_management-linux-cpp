
#ifndef RANDOM_ZERO_WRITE_HPP
#define RANDOM_ZERO_WRITE_HPP

#include <overwrite_method.hpp>
#include <sanitization_method_registry.hpp>

class RandomZeroWrite: public OverwriteMethod, private AutoRegisterMethod<RandomZeroWrite> {
public:
    [[nodiscard]] static RandomZeroWrite& shared();

    RandomZeroWrite(const RandomZeroWrite&) = delete;
    RandomZeroWrite(RandomZeroWrite&&) = delete;
    RandomZeroWrite& operator=(const RandomZeroWrite&) = delete;
    RandomZeroWrite& operator=(RandomZeroWrite&&) = delete;

private:
    RandomZeroWrite();
};

#endif // RANDOM_ZERO_WRITE_HPP

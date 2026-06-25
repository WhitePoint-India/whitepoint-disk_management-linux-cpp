
#ifndef RANDOM_WRITE_HPP
#define RANDOM_WRITE_HPP

#include <overwrite_method.hpp>
#include <sanitization_method_registry.hpp>

class RandomWrite: public OverwriteMethod, private AutoRegisterMethod<RandomWrite> {
public:
    [[nodiscard]] static RandomWrite& shared();

    RandomWrite(const RandomWrite&) = delete;
    RandomWrite(RandomWrite&&) = delete;
    RandomWrite& operator=(const RandomWrite&) = delete;
    RandomWrite& operator=(RandomWrite&&) = delete;

private:
    RandomWrite();
};

#endif // RANDOM_WRITE_HPP

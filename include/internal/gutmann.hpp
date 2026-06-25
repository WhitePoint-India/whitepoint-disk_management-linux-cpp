
#ifndef GUTMANN_HPP
#define GUTMANN_HPP

#include <vector>

#include <overwrite_method.hpp>
#include <sanitization_method_registry.hpp>

class Gutmann: public OverwriteMethod, private AutoRegisterMethod<Gutmann> {
public:
    [[nodiscard]] static Gutmann& shared();

    Gutmann(const Gutmann&) = delete;
    Gutmann(Gutmann&&) = delete;
    Gutmann& operator=(const Gutmann&) = delete;
    Gutmann& operator=(Gutmann&&) = delete;

private:
    Gutmann();
    static std::vector<Pass> buildPasses();
};

#endif // GUTMANN_HPP

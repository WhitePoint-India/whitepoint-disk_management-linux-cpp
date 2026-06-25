
#ifndef NSA_HPP
#define NSA_HPP

#include <overwrite_method.hpp>
#include <sanitization_method_registry.hpp>

class NSA: public OverwriteMethod, private AutoRegisterMethod<NSA> {
public:
    [[nodiscard]] static NSA& shared();

    NSA(const NSA&) = delete;
    NSA(NSA&&) = delete;
    NSA& operator=(const NSA&) = delete;
    NSA& operator=(NSA&&) = delete;

private:
    NSA();
};

#endif // NSA_HPP

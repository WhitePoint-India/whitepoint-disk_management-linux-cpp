
#ifndef DOD_5220_28_M_HPP
#define DOD_5220_28_M_HPP

#include <overwrite_method.hpp>
#include <sanitization_method_registry.hpp>

class DoD522028M: public OverwriteMethod, private AutoRegisterMethod<DoD522028M> {
public:
    [[nodiscard]] static DoD522028M& shared();

    DoD522028M(const DoD522028M&) = delete;
    DoD522028M(DoD522028M&&) = delete;
    DoD522028M& operator=(const DoD522028M&) = delete;
    DoD522028M& operator=(DoD522028M&&) = delete;

private:
    DoD522028M();
};

#endif // DOD_5220_28_M_HPP

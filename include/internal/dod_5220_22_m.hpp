
#ifndef DOD_5220_22_M_HPP
#define DOD_5220_22_M_HPP

#include <overwrite_method.hpp>
#include <sanitization_method_registry.hpp>

class DoD522022M: public OverwriteMethod, private AutoRegisterMethod<DoD522022M> {
public:
    [[nodiscard]] static DoD522022M& shared();

    DoD522022M(const DoD522022M&) = delete;
    DoD522022M(DoD522022M&&) = delete;
    DoD522022M& operator=(const DoD522022M&) = delete;
    DoD522022M& operator=(DoD522022M&&) = delete;

private:
    DoD522022M();
};

#endif // DOD_5220_22_M_HPP

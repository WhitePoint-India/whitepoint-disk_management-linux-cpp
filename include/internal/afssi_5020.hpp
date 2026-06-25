
#ifndef AFSSI_5020_HPP
#define AFSSI_5020_HPP

#include <overwrite_method.hpp>
#include <sanitization_method_registry.hpp>

class AFSSI5020: public OverwriteMethod, private AutoRegisterMethod<AFSSI5020> {
public:
    [[nodiscard]] static AFSSI5020& shared();

    AFSSI5020(const AFSSI5020&) = delete;
    AFSSI5020(AFSSI5020&&) = delete;
    AFSSI5020& operator=(const AFSSI5020&) = delete;
    AFSSI5020& operator=(AFSSI5020&&) = delete;

private:
    AFSSI5020();
};

#endif // AFSSI_5020_HPP

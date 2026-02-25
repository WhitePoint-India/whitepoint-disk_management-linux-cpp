
#ifndef NIST_800_88_PURGE_HPP
#define NIST_800_88_PURGE_HPP

#include <operations.hpp>

namespace DiskManagement {

class NIST80088Purge: public SingletonMethod<NIST80088Purge> {
    friend class SingletonMethod<NIST80088Purge>;
    NIST80088Purge();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;
};

} // namespace DiskManagement

#endif // NIST_800_88_PURGE_HPP

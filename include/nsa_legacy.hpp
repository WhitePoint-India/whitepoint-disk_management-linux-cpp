
#ifndef NSA_LEGACY_HPP
#define NSA_LEGACY_HPP

#include <operations.hpp>

namespace DiskManagement {

class NSALegacy: public SingletonMethod<NSALegacy> {
    friend class SingletonMethod<NSALegacy>;
    NSALegacy();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;
};

} // namespace DiskManagement

#endif // NSA_LEGACY_HPP

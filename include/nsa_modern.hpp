
#ifndef NSA_MODERN_HPP
#define NSA_MODERN_HPP

#include <operations.hpp>

namespace DiskManagement {

class NSAModern: public SingletonMethod<NSAModern> {
    friend class SingletonMethod<NSAModern>;
    NSAModern();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;
};

} // namespace DiskManagement

#endif // NSA_MODERN_HPP

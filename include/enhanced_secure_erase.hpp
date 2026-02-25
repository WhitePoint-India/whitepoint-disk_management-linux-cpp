
#ifndef ENHANCED_SECURE_ERASE_HPP
#define ENHANCED_SECURE_ERASE_HPP

#include <operations.hpp>

namespace DiskManagement {

class EnhancedSecureErase: public SingletonMethod<EnhancedSecureErase> {
    friend class SingletonMethod<EnhancedSecureErase>;
    EnhancedSecureErase();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;
};

} // namespace DiskManagement

#endif // ENHANCED_SECURE_ERASE_HPP

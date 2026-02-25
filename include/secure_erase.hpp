
#ifndef SECURE_ERASE_HPP
#define SECURE_ERASE_HPP

#include <operations.hpp>

namespace DiskManagement {

class SecureErase: public SingletonMethod<SecureErase> {
    friend class SingletonMethod<SecureErase>;
    SecureErase();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;
};

} // namespace DiskManagement

#endif // SECURE_ERASE_HPP

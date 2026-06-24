
#ifndef ATA_SECURE_ERASABLE_HPP
#define ATA_SECURE_ERASABLE_HPP

#include <functional>

class ATASecureErasable {

protected:
    virtual bool supportsEnhancedErase() const = 0;

public:

    using Callback = std::function<void(const double fractionCompleted)>;

    virtual ~ATASecureErasable() noexcept = default;

    [[nodiscard]] virtual bool isFrozen() const = 0;
    virtual void unfreeze() = 0;

    virtual void secureErase(Callback callback) = 0;
    virtual void secureEraseEnhanced(Callback callback) = 0;
};

#endif // ATA_SECURE_ERASABLE_HPP

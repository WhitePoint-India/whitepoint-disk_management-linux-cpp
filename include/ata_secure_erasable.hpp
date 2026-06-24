
#ifndef ATA_SECURE_ERASABLE_HPP
#define ATA_SECURE_ERASABLE_HPP

class ATASecureErasable {

protected:
    virtual bool supportsEnhancedErase() const = 0;

public:
    virtual ~ATASecureErasable() noexcept = default;

    [[nodiscard]] virtual bool isFrozen() const = 0;
    virtual void unfreeze() = 0;

    virtual void secureErase() = 0;
    virtual void secureEraseEnhanced() = 0;
};

#endif // ATA_SECURE_ERASABLE_HPP


#ifndef ATA_SECURE_ERASABLE_HPP
#define ATA_SECURE_ERASABLE_HPP

class ATASecureErasable {
public:
    virtual ~ATASecureErasable() noexcept = default;

    [[nodiscard]] virtual bool isFrozen() const = 0;
    virtual void unfreeze() = 0;
    virtual void secureEraseUnit(bool enhanced) = 0;
};

#endif // ATA_SECURE_ERASABLE_HPP

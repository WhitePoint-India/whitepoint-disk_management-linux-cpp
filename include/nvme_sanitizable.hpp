
#ifndef NVME_SANITIZABLE_HPP
#define NVME_SANITIZABLE_HPP

#include <functional>

class NVMeSanitizable {
public:
    // NVMe Sanitize Action (SANACT) — the erase mechanisms. Values match the
    // NVMe specification's SANACT field.
    enum class Action {
        BlockErase  = 2,
        Overwrite   = 3,
        CryptoErase = 4,
    };

    // Reports the completion fraction in [0.0, 1.0] as the operation proceeds.
    using Callback = std::function<void(const double fractionCompleted)>;

    virtual ~NVMeSanitizable() noexcept = default;

    virtual void sanitize(Action action, Callback callback) = 0;
    virtual void formatNVM(Callback callback) = 0;
};

#endif // NVME_SANITIZABLE_HPP

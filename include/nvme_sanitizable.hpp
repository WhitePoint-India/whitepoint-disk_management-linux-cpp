
#ifndef NVME_SANITIZABLE_HPP
#define NVME_SANITIZABLE_HPP

class NVMeSanitizable {
public:
    virtual ~NVMeSanitizable() noexcept = default;

    virtual void nvmeSanitize(int sanitizeAction) = 0;
    virtual void nvmeFormatNVM() = 0;
};

#endif // NVME_SANITIZABLE_HPP


#ifndef NIST_800_88_CLEAR_HPP
#define NIST_800_88_CLEAR_HPP

#include <operations.hpp>

namespace DiskManagement {

class NIST80088Clear: public SingletonMethod<NIST80088Clear> {
    friend class SingletonMethod<NIST80088Clear>;
    NIST80088Clear();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;

    const std::vector<Stage>& getStages() const override;

    void deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) override;
    void deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) override;
};

} // namespace DiskManagement

#endif // NIST_800_88_CLEAR_HPP

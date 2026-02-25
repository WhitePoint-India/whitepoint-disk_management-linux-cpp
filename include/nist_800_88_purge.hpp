
#ifndef NIST_800_88_PURGE_HPP
#define NIST_800_88_PURGE_HPP

#include <operations.hpp>

namespace DiskManagement {

class NIST80088Purge: public SingletonMethod<NIST80088Purge>, private Verifiable {
    friend class SingletonMethod<NIST80088Purge>;
    NIST80088Purge();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;

    void deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) override;
    void deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) override;
    void deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) override;

private:
    void execute(Disk& disk, DiskDeleteMethod::Delegate& delegate);
};

} // namespace DiskManagement

#endif // NIST_800_88_PURGE_HPP

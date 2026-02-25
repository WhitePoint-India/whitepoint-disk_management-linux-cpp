
#ifndef NSA_LEGACY_HPP
#define NSA_LEGACY_HPP

#include <operations.hpp>

namespace DiskManagement {

class NSALegacy: public SingletonMethod<NSALegacy>, private Writable {
    friend class SingletonMethod<NSALegacy>;
    NSALegacy();
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

#endif // NSA_LEGACY_HPP


#ifndef DOD_5220_28_M_HPP
#define DOD_5220_28_M_HPP

#include <operations.hpp>

namespace DiskManagement {

class DoD522028M: public SingletonMethod<DoD522028M>, private Writable, private Verifiable {
    friend class SingletonMethod<DoD522028M>;
    DoD522028M();
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

#endif // DOD_5220_28_M_HPP

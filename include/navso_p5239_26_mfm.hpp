
#ifndef NAVSO_P5239_26_MFM_HPP
#define NAVSO_P5239_26_MFM_HPP

#include <operations.hpp>

namespace DiskManagement {

class NAVSOP523926MFM: public SingletonMethod<NAVSOP523926MFM>, private Writable {
    friend class SingletonMethod<NAVSOP523926MFM>;
    NAVSOP523926MFM();
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

#endif // NAVSO_P5239_26_MFM_HPP

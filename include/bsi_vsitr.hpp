
#ifndef BSI_VSITR_HPP
#define BSI_VSITR_HPP

#include <operations.hpp>

namespace DiskManagement {

class BSIVSITR: public SingletonMethod<BSIVSITR>, private Writable {
    friend class SingletonMethod<BSIVSITR>;
    BSIVSITR();
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

#endif // BSI_VSITR_HPP

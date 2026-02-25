
#ifndef ZERO_WRITE_HPP
#define ZERO_WRITE_HPP

#include <operations.hpp>

namespace DiskManagement {

class ZeroWrite: public SingletonMethod<ZeroWrite>, private Writable {
    friend class SingletonMethod<ZeroWrite>;
    ZeroWrite();
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

#endif // ZERO_WRITE_HPP

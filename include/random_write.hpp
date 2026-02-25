
#ifndef RANDOM_WRITE_HPP
#define RANDOM_WRITE_HPP

#include <operations.hpp>

namespace DiskManagement {

class RandomWrite: public SingletonMethod<RandomWrite>, private Writable {
    friend class SingletonMethod<RandomWrite>;
    RandomWrite();
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

#endif // RANDOM_WRITE_HPP


#ifndef RANDOM_ZERO_WRITE_HPP
#define RANDOM_ZERO_WRITE_HPP

#include <operations.hpp>

namespace DiskManagement {

class RandomZeroWrite: public SingletonMethod<RandomZeroWrite>, private Writable {
    friend class SingletonMethod<RandomZeroWrite>;
    RandomZeroWrite();
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

#endif // RANDOM_ZERO_WRITE_HPP

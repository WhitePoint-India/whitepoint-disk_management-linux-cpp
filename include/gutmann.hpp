
#ifndef GUTMANN_HPP
#define GUTMANN_HPP

#include <operations.hpp>

namespace DiskManagement {

class Gutmann: public SingletonMethod<Gutmann>, private Writable {
    friend class SingletonMethod<Gutmann>;
    Gutmann();
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

#endif // GUTMANN_HPP

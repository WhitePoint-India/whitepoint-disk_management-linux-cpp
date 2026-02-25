
#ifndef ZERO_WRITE_HPP
#define ZERO_WRITE_HPP

#include <operations.hpp>

namespace DiskManagement {

class ZeroWrite: public SingletonMethod<ZeroWrite> {
    friend class SingletonMethod<ZeroWrite>;
    ZeroWrite();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;

    const std::vector<Stage>& getStages() const override;

    void deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) override;
};

} // namespace DiskManagement

#endif // ZERO_WRITE_HPP

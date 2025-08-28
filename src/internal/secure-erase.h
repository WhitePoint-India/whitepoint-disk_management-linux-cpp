#ifndef SECURE_ERASE_H
#define SECURE_ERASE_H

#include <disk-management>

namespace DiskManagement {
namespace Internal {

class SecureErase : public DiskDeleteMethod {
public:
    void deleteDisk(Disk& disk) const override;
};

} // namespace Internal
} // namespace DiskManagement

#endif // SECURE_ERASE_H
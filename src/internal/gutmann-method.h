#ifndef GUTMANN_METHOD_H
#define GUTMANN_METHOD_H

#include <disk-management>

namespace DiskManagement {
namespace Internal {

class GutmannMethod : public DiskDeleteMethod {
public:
    void deleteDisk(Disk& disk) const override;
};

} // namespace Internal
} // namespace DiskManagement

#endif // GUTMANN_METHOD_H
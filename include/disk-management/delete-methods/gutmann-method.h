
#ifndef GUTMANN_METHOD_H
#define GUTMANN_METHOD_H

#include <disk-management/delete.h>
#include <disk-management/details.h>

class GutmannMethod : public DiskManagement::DiskDeleteMethod {
public:
    void deleteDisk(DiskManagement::Disk disk) override;
};

#endif // GUTMANN_METHOD_H
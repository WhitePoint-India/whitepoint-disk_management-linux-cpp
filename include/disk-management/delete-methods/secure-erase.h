
#ifndef SECURE_ERASE_H
#define SECURE_ERASE_H

#include <disk-management/delete.h>
#include <disk-management/details.h>

class SecureErase : public DiskManagement::DiskDeleteMethod {
public:
    void deleteDisk(DiskManagement::Disk disk) override;
};

#endif // SECURE_ERASE_H

#ifndef DISK_DELETE_METHOD_H
#define DISK_DELETE_METHOD_H

namespace DiskManagement {
    // Forward declaration
    class Disk;

    class DiskDeleteMethod {
    public:
       virtual void deleteDisk(Disk disk) = 0;
    };
}

#endif // DISK_DELETE_METHOD_H
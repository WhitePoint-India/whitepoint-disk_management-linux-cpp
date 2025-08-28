
#ifndef DISK_DETAILS_H
#define DISK_DETAILS_H

#include <string>
#include <disk-management/delete.h>

namespace DiskManagement {

    enum DiskState {
        READY,
        FROZEN,
    };

    class Disk {

    public:
        std::string serial;
        std::string model;
        std::string path;
        std::string description;
        unsigned long long size;
        unsigned long long sectorCount;
        DiskState state;

        Disk();

        bool isFrozen();
        void unfreeze();
        void deleteDisk(DiskDeleteMethod& method);
    };

    Disk* fetchDisks();
}

#endif // DISK_DETAILS_H
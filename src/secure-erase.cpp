
#include <iostream>
#include "internal/secure-erase.h"

void DiskManagement::Internal::SecureErase::deleteDisk(DiskManagement::Disk& disk) const {
    std::cout << "Secure erasing disk: " << disk.model 
              << " at " << disk.path << std::endl;
}
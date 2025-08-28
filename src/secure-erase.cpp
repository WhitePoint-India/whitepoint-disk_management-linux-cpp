
#include <iostream>
#include <disk-management/delete-methods/secure-erase.h>

void SecureErase::deleteDisk(DiskManagement::Disk disk) {
    
    std::cout << "Secure Erase...." << disk.path << std::endl;
}
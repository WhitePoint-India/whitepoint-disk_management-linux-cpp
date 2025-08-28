
#include <iostream>
#include <disk-management/delete-methods/gutmann-method.h>

void GutmannMethod::deleteDisk(DiskManagement::Disk disk) {
    
    std::cout << "Gutmann method erase...." << disk.path <<std::endl;
}

#include <iostream>
#include "internal/gutmann-method.h"

void DiskManagement::Internal::GutmannMethod::deleteDisk(DiskManagement::Disk& disk) const {
    std::cout << "Gutmann method erasing disk: " << disk.model 
              << " at " << disk.path << std::endl;
}
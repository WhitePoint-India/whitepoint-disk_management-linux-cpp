
#include <iostream>
#include "internal/secure-erase.h"

void DiskManagement::Internal::SecureErase::deleteDisk(DiskManagement::Disk& disk) const {
    
    std::cout << "--- Disk Information ---" << std::endl;
    
    std::cout << "Serial : " << disk.serial << std::endl;
    std::cout << "Model : " << disk.model << std::endl;
    std::cout << "Path : " << disk.path<< std::endl;
    std::cout << "Description : " << disk.description << std::endl;
    std::cout << "Size : " << disk.size << std::endl;
    std::cout << "Sector count : " << disk.getSectorCount() << std::endl;
    std::cout << "State : " << disk.state << std::endl;


    std::cout << "------------------------" << std::endl;
}
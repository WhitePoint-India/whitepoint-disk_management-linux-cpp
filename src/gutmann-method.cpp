
#include <iostream>
#include <disk-management>

void DiskManagement::GutmannMethod::deleteDisk(DiskManagement::ATADisk& disk) const {
    
    std::cout << "--- Performing Gutmann Erase on ATA Disk ---" << std::endl;
        
    std::cout << "Serial : " << disk.serial << std::endl;
    std::cout << "Model : " << disk.model << std::endl;
    std::cout << "Path : " << disk.path << std::endl;
    std::cout << "Description : " << disk.description << std::endl;
    std::cout << "Size : " << disk.size << std::endl;
    std::cout << "Sector count : " << disk.getSectorCount() << std::endl;
    std::cout << "Frozen : " << (disk.isFrozen() ? "Yes" : "No") << std::endl;

    if (disk.isFrozen()) {
        std::cout << "Disk is frozen. Unfreezing..." << std::endl;
        disk.unfreeze();
    }

    // TODO: Implement actual secure erase command
    std::cout << "Secure erase would be performed here" << std::endl;
    
    std::cout << "------------------------" << std::endl;
}


void DiskManagement::GutmannMethod::deleteDisk(DiskManagement::NVMeDisk& disk) const {
    
    std::cout << "--- Performing Gutmann Erase on NVMe Disk ---" << std::endl;
        
    std::cout << "Serial : " << disk.serial << std::endl;
    std::cout << "Model : " << disk.model << std::endl;
    std::cout << "Path : " << disk.path << std::endl;
    std::cout << "Description : " << disk.description << std::endl;
    std::cout << "Size : " << disk.size << std::endl;
    std::cout << "Sector count : " << disk.getSectorCount() << std::endl;

    // TODO: Implement actual secure erase command
    std::cout << "Secure erase would be performed here" << std::endl;
    
    std::cout << "------------------------" << std::endl;
}
#include <iomanip>
#include <iostream>
#include <disk-management>
#include <memory>

int main() {
    
    std::vector<std::unique_ptr<DiskManagement::Disk>> disks = DiskManagement::fetchDisks();
    
    for (const std::unique_ptr<DiskManagement::Disk>& diskPtr : disks) {

        std::cout << "--- Disk Information ---" << std::endl;
            
        std::cout << "Serial : " << diskPtr->serial << std::endl;
        std::cout << "Model : " << diskPtr->model << std::endl;
        std::cout << "Path : " << diskPtr->path << std::endl;
        std::cout << "Description : " << diskPtr->description << std::endl;
        std::cout << "Size : " << diskPtr->size << std::endl;
        std::cout << "Sector count : " << diskPtr->getSectorCount() << std::endl;
        
        std::cout << "------------------------" << std::endl;
    }

    return 0;
}
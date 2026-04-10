
#include <iostream>
#include <disk_management>

int main() {

    auto disks = DiskManagement::fetchDisks();

    if (disks.empty()) {
        std::cout << "No disks found." << std::endl;
        return 0;
    }

    std::cout << "Found " << disks.size() << " disk(s):\n" << std::endl;

    for (const auto& disk: disks) {
        std::cout << "  Path:         " << disk->getPath() << "\n";
        std::cout << "  Model:        " << disk->getModel() << "\n";
        std::cout << "  Serial:       " << disk->getSerial() << "\n";
        std::cout << "  Description:  " << disk->getDescription() << "\n";
        std::cout << "  Size:         " << disk->getSize() / (1000ULL * 1000 * 1000) << " GB\n";
        std::cout << "  Sector Size:  " << disk->getSectorSize() << " bytes\n";
        std::cout << "  Sector Count: " << disk->getSectorCount() << "\n";
        std::cout << std::endl;
    }

    auto selectedMethod = DiskManagement::methods[0];

    

    return 0;
}

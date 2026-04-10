
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
        std::visit([](const Disk& d) {
            std::cout << "  Path:         " << d.getPath() << "\n";
            std::cout << "  Model:        " << d.getModel() << "\n";
            std::cout << "  Serial:       " << d.getSerial() << "\n";
            std::cout << "  Description:  " << d.getDescription() << "\n";
            std::cout << "  Size:         " << d.getSize() / (1000ULL * 1000 * 1000) << " GB\n";
            std::cout << "  Sector Size:  " << d.getSectorSize() << " bytes\n";
            std::cout << "  Sector Count: " << d.getSectorCount() << "\n";
            std::cout << std::endl;
        }, disk);
    }

    DiskSanitizationInterface& selectedMethod = DiskManagement::methods[0];

    for (auto& disk : disks) {
        selectedMethod.deleteDisk(disk);
    }

    return 0;
}

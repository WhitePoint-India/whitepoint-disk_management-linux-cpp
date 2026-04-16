
#include <iostream>
#include <disk_management>
#include <sanitization_stage.hpp>

int main() {
    std::vector<DiskVariant> disks = DiskManagement::fetchDisks();

    if (disks.empty()) {
        std::cout << "No disks found." << std::endl;
        return 0;
    }

    std::cout << "Found " << disks.size() << " disk(s):\n" << std::endl;

    for (const DiskVariant& disk: disks) {
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

    DiskSanitizationInterface& method = DiskManagement::methods[0];

    for (auto& disk : disks) {
        method.sanitize(disk, [](const SanitizationProgress progress) {
            std::cout << "[" << (progress.currentIndex() + 1) << "/" << progress.totalStageCount() << " " << progress.getStage().title() << "] " << progress.fractionCompleted() * 100 << "% - " << progress.getStage().description() << std::endl;
        });
    }
    
    return 0;
}

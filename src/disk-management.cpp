#include <disk-management>
#include "internal/secure-erase.h"
#include "internal/gutmann-method.h"

namespace {
    const DiskManagement::Internal::SecureErase secureEraseInstance;
    const DiskManagement::Internal::GutmannMethod gutmannMethodInstance;
}

const DiskManagement::DiskDeleteMethod& DiskManagement::SECURE_ERASE_METHOD = secureEraseInstance;
const DiskManagement::DiskDeleteMethod& DiskManagement::GUTMANN_METHOD = gutmannMethodInstance;

std::vector<DiskManagement::Disk> DiskManagement::fetchDisks() {
    return {
        Disk(
            "SX9J109PBCJ05X",
            "Samsung SSD 250 G",
            "/dev/sda1",
            "This is a samsung drive ssd",
            1099511627776ULL,
            2024ULL,
            DiskState::READY
        ),
        Disk(
            "CG7F726KDGH09Y",
            "WD PASSPORT 1 TB",
            "/dev/sda2",
            "This is a WD hard drive",
            1099511627776ULL,
            2024ULL,
            DiskState::READY
        )
    };
}
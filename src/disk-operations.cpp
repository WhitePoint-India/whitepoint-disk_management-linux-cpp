
#include <disk-management>

namespace {

const DiskOperations::SecureErase secureEraseInstance;
const DiskOperations::GutmannMethod gutmannMethodInstance;

}

const DiskOperations::SecureErase& DiskOperations::SECURE_ERASE_METHOD = secureEraseInstance;
const DiskOperations::GutmannMethod& DiskOperations::GUTMANN_METHOD = gutmannMethodInstance;
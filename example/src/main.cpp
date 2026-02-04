
#include <memory>
#include <iomanip>
#include <iostream>
#include <disk-management>

void callbackATASecureErase(DiskOperations::SecureErase::OperationStage stage, const DiskOperations::Progress& progress) {
    std::cout << "ATA SecureErase Progress :: " << progress.fractionCompleted() << std::endl;
}

void callbackATAEnhancedSecureErase(DiskOperations::EnhancedSecureErase::OperationStage stage, const DiskOperations::Progress& progress) {
    std::cout << "ATA EnhancedSecureErase Progress :: " << progress.fractionCompleted() << std::endl;
}

void callbackATAGutmannMethod(DiskOperations::GutmannMethod::OperationStage stage, const DiskOperations::Progress& progress) {
    std::cout << "ATA GutmannMethod Progress :: " << progress.fractionCompleted() << std::endl;
}

void callbackATA_NIST_800_88(DiskOperations::NIST_800_88::OperationStage stage, const DiskOperations::Progress& progress) {
    std::cout << "ATA NIST_800_88 Progress :: " << progress.fractionCompleted() << std::endl;
}

void callbackNVMeSecureErase(DiskOperations::SecureErase::OperationStage stage, const DiskOperations::Progress& progress) {
    std::cout << "NVMe SecureErase Progress :: " << progress.fractionCompleted() << std::endl;
}

void callbackNVMeEnhancedSecureErase(DiskOperations::EnhancedSecureErase::OperationStage stage, const DiskOperations::Progress& progress) {
    std::cout << "NVMe EnhancedSecureErase Progress :: " << progress.fractionCompleted() << std::endl;
}

void callbackNVMeGutmannMethod(DiskOperations::GutmannMethod::OperationStage stage, const DiskOperations::Progress& progress) {
    std::cout << "NVMe GutmannMethod Progress :: " << progress.fractionCompleted() << std::endl;
}

void callbackNVMe_NIST_800_88(DiskOperations::NIST_800_88::OperationStage stage, const DiskOperations::Progress& progress) {
    std::cout << "NVMe NIST_800_88 Progress :: " << progress.fractionCompleted() << std::endl;
}

int main() {
    std::vector<std::unique_ptr<DiskManagement::Disk>> disks = DiskManagement::fetchDisks();
    for (const std::unique_ptr<DiskManagement::Disk>& diskPtr : disks) {
        if (auto ataDisk = dynamic_cast<DiskManagement::ATADisk*>(diskPtr.get())) {
            ataDisk->deleteDisk(DiskManagement::SECURE_ERASE, callbackATASecureErase);
            ataDisk->deleteDisk(DiskManagement::ENHANCED_SECURE_ERASE, callbackATAEnhancedSecureErase);
            ataDisk->deleteDisk(DiskManagement::GUTMANN_METHOD, callbackATAGutmannMethod);
            ataDisk->deleteDisk(DiskManagement::NIST_800_88, callbackATA_NIST_800_88);
        }
        else if (auto nvmeDisk = dynamic_cast<DiskManagement::NVMeDisk*>(diskPtr.get())) {
            ataDisk->deleteDisk(DiskManagement::SECURE_ERASE, callbackNVMeSecureErase);
            ataDisk->deleteDisk(DiskManagement::ENHANCED_SECURE_ERASE, callbackNVMeEnhancedSecureErase);
            ataDisk->deleteDisk(DiskManagement::GUTMANN_METHOD, callbackNVMeGutmannMethod);
            ataDisk->deleteDisk(DiskManagement::NIST_800_88, callbackNVMe_NIST_800_88);
        }
    }
    return 0;
}

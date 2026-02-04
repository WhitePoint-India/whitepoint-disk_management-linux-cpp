
#include <iostream>
#include <disk-operations>

namespace DiskOperations {

    namespace EnhancedSecureErase {

        void Operation::deleteDisk(DiskManagement::ATADisk& disk, AnyDeleteOperation<DiskManagement::ATADisk, OperationStage>::Callback callback) const {
            for (int i = 0; i <= 100; i++) {
                Progress progress = Progress(i, 100);
                callback(OperationStage::ERASURE, progress);
            }
        }

        void Operation::deleteDisk(DiskManagement::NVMeDisk& disk, AnyDeleteOperation<DiskManagement::NVMeDisk, OperationStage>::Callback callback) const {
            for (int i = 0; i <= 100; i++) {
                Progress progress = Progress(i, 100);
                callback(OperationStage::ERASURE, progress);
            }
        }
    } // NIST_800_88
} // DiskOperations
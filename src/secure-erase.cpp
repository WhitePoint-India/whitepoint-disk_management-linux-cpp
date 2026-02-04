
#include <iostream>
#include <disk-management>

// void DiskManagement::SecureErase::deleteDisk(DiskManagement::ATADisk& disk) const {
    
//     std::string commandSetPassword = "hdparm --user-master u --security-set-pass whitepoint " + disk.path + " > /dev/null 2>&1";
//     std::string commandPerformSecureErase = "hdparm --user-master u --security-erase whitepoint " + disk.path + " > /dev/null 2>&1";

//     system(commandSetPassword.c_str());

//     system(commandPerformSecureErase.c_str());
// }


// void DiskManagement::SecureErase::deleteDisk(DiskManagement::NVMeDisk& disk) const {
    
//     std::string commandSecureErase = "nvme format " + disk.path + " -s 1 -f > /dev/null 2>&1";

//     system(commandSecureErase.c_str());
// }


void DiskOperations::SecureErase::deleteDisk(DiskManagement::ATADisk& disk, DiskOperations::ATADisk::DeleteOperation::Callback callback) const {
    for (int i = 0; i <= 100; i++) {
        Progress progress = Progress(i, 100);
        callback(ATADisk::Stage::ERASURE, progress);
    }
}

void DiskOperations::SecureErase::deleteDisk(DiskManagement::NVMeDisk& disk, DiskOperations::NVMeDisk::DeleteOperation::Callback callback) const {
    for (int i = 0; i <= 100; i++) {
        Progress progress = Progress(i, 100);
        callback(NVMeDisk::Stage::ERASURE, progress);
    }
}
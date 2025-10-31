
#include <disk-management>
#include <iostream>

void DiskManagement::SecureErase::deleteDisk(DiskManagement::ATADisk& disk) const {
    
    std::string commandSetPassword = "hdparm --user-master u --security-set-pass whitepoint " + disk.path + " > /dev/null 2>&1";
    std::string commandPerformSecureErase = "hdparm --user-master u --security-erase whitepoint " + disk.path + " > /dev/null 2>&1";

    system(commandSetPassword.c_str());

    system(commandPerformSecureErase.c_str());
}


void DiskManagement::SecureErase::deleteDisk(DiskManagement::NVMeDisk& disk) const {
    
    std::string commandSecureErase = "nvme format " + disk.path + " -s 1 -f";

    system(commandSecureErase.c_str());
}
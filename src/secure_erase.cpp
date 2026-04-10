
#include <secure_erase.hpp>

SecureErase::SecureErase()
    : DiskSanitizationInterface("secure_erase", "Secure Erase") {}

SecureErase& SecureErase::shared() {
    static SecureErase instance;
    return instance;
}

/// @brief
/// @param disk
void SecureErase::deleteDisk(NVMeDisk& disk) {

}

/// @brief 
/// @param disk
void SecureErase::deleteDisk(ATADisk& disk) {
    
}

#include <nist_clear.hpp>

NISTClear::NISTClear() : DiskSanitizationInterface("NIST_800_88_CLEAR") {
    
}

NISTClear& NISTClear::shared() {
    static NISTClear instance;
    return instance;
}

void NISTClear::sanitize(DiskVariant& disk) {
    std::visit([this](auto& d) { deleteDisk(d); }, disk);
}

void NISTClear::deleteDisk(NVMeDisk& /* disk */) {

}

void NISTClear::deleteDisk(ATADisk& /* disk */) {

}
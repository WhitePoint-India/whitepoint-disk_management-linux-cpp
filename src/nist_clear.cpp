
#include <nist_clear.hpp>

NISTClear::NISTClear()
    : DiskSanitizationInterface("nist_800_88_clear", "NIST 800-88 Clear") {}

NISTClear& NISTClear::shared() {
    static NISTClear instance;
    return instance;
}

void NISTClear::deleteDisk(NVMeDisk& disk) {

}

void NISTClear::deleteDisk(ATADisk& disk) {

}
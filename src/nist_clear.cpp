
#include <nist_clear.hpp>

NISTClear::NISTClear() : DiskSanitizationInterface("NIST_800_88_CLEAR") {

}

NISTClear& NISTClear::shared() {
    static NISTClear instance;
    return instance;
}

void NISTClear::sanitize(DiskVariant& disk, Callback callback) {
    std::visit([this, callback](auto& d) { deleteDisk(d, callback); }, disk);
}

void NISTClear::deleteDisk(NVMeDisk& disk, Callback callback) {
    Stage stage1 = Stage::PASS_1;
    callback(stage1);
    Stage stage2 = Stage::PASS_2;
    callback(stage2);
    Stage stage3 = Stage::PASS_3;
    callback(stage3);
}

void NISTClear::deleteDisk(ATADisk& disk, Callback callback) {
    Stage stage1 = Stage::PASS_1;
    callback(stage1);
    Stage stage2 = Stage::PASS_2;
    callback(stage2);
    Stage stage3 = Stage::PASS_3;
    callback(stage3);
}

std::string NISTClear::Stage::title() const {
    switch (value_) {
        case PASS_1: return "Pass 1";
        case PASS_2: return "Pass 2";
        case PASS_3: return "Pass 3";
        default: return "Unknown Stage";
    }
}

std::string NISTClear::Stage::description() const {
    switch (value_) {
        case PASS_1: return "First pass of NIST clear sanitization.";
        case PASS_2: return "Second pass of NIST clear sanitization.";
        case PASS_3: return "Third pass of NIST clear sanitization.";
        default: return "Unknown stage for NIST clear sanitization.";
    }
}

std::string NISTClear::Stage::localizedTitle() const {
    return "NIST Clear Stage";
}

std::string NISTClear::Stage::localizedDescription() const {
    return "A stage for NIST clear sanitization.";
}
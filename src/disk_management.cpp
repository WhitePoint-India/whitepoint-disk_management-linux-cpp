
#include <disk_management>
#include <lshw_hardware_detector.hpp>

// Composition root: trigger singleton construction so methods self-register.
// With static libraries, the linker drops translation units that don't resolve
// any undefined symbols, so auto-registration alone is not sufficient.
#include <nist_clear.hpp>
#include <nist_purge.hpp>
#include <secure_erase.hpp>
#include <secure_erase_enhanced.hpp>

namespace {

void ensureMethodsRegistered() {
    [[maybe_unused]] static bool registered = [] {
        NISTClear::shared();
        NISTPurge::shared();
        SecureErase::shared();
        EnhancedSecureErase::shared();
        return true;
    }();
}

} // anonymous namespace

namespace DiskManagement {

const std::vector<std::reference_wrapper<DiskSanitizationInterface>>& methods() {
    ensureMethodsRegistered();
    return SanitizationMethodRegistry::instance().methods();
}

std::vector<std::unique_ptr<Disk>> fetchDisks() {
    LshwHardwareDetector detector;
    return detector.fetchDisks();
}

std::vector<std::unique_ptr<Disk>> fetchDisks(HardwareDetector& detector) {
    return detector.fetchDisks();
}

} // namespace DiskManagement

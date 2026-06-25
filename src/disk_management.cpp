
#include <disk_management>
#include <lshw_hardware_detector.hpp>

// Composition root: trigger singleton construction so methods self-register.
// With static libraries, the linker drops translation units that don't resolve
// any undefined symbols, so auto-registration alone is not sufficient.
#include <nist_clear.hpp>
#include <nist_purge.hpp>
#include <secure_erase.hpp>
#include <secure_erase_enhanced.hpp>
#include <zero_write.hpp>
#include <random_write.hpp>
#include <random_zero_write.hpp>
#include <nsa.hpp>
#include <bit_toggle.hpp>
#include <dod_5220_28_m.hpp>
#include <dod_5220_22_m.hpp>
#include <afssi_5020.hpp>
#include <navso_p5239_26_mfm.hpp>
#include <navso_p5239_26_rll.hpp>
#include <bsi_vsitr.hpp>
#include <gutmann.hpp>

namespace {

void ensureMethodsRegistered() {
    [[maybe_unused]] static bool registered = [] {
        NISTClear::shared();
        NISTPurge::shared();
        SecureErase::shared();
        EnhancedSecureErase::shared();
        ZeroWrite::shared();
        RandomWrite::shared();
        RandomZeroWrite::shared();
        NSA::shared();
        BitToggle::shared();
        DoD522028M::shared();
        DoD522022M::shared();
        AFSSI5020::shared();
        NAVSOP523926MFM::shared();
        NAVSOP523926RLL::shared();
        BSIVSITR::shared();
        Gutmann::shared();
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

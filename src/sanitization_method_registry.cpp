
#include <algorithm>

#include <sanitization_method_registry.hpp>
#include <disk_sanitization_interface.hpp>

SanitizationMethodRegistry& SanitizationMethodRegistry::instance() {
    static SanitizationMethodRegistry registry;
    return registry;
}

void SanitizationMethodRegistry::add(DiskSanitizationInterface& method) {
    // Keep methods_ sorted by grade at all times, so callers (registry itself
    // and consumers of methods()) never need to sort it themselves.
    const auto position = std::upper_bound(
        methods_.begin(), methods_.end(), method,
        [](const DiskSanitizationInterface& lhs, const DiskSanitizationInterface& rhs) {
            return lhs.grade() < rhs.grade();
        });
    methods_.emplace(position, method);
}

const std::vector<std::reference_wrapper<DiskSanitizationInterface>>& SanitizationMethodRegistry::methods() const {
    return methods_;
}

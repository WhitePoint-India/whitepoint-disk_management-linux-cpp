
#include <sanitization_method_registry.hpp>

SanitizationMethodRegistry& SanitizationMethodRegistry::instance() {
    static SanitizationMethodRegistry registry;
    return registry;
}

void SanitizationMethodRegistry::add(DiskSanitizationInterface& method) {
    methods_.emplace_back(method);
}

const std::vector<std::reference_wrapper<DiskSanitizationInterface>>& SanitizationMethodRegistry::methods() const {
    return methods_;
}

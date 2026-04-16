
#ifndef SANITIZATION_METHOD_REGISTRY_HPP
#define SANITIZATION_METHOD_REGISTRY_HPP

#include <vector>
#include <functional>

class DiskSanitizationInterface;

class SanitizationMethodRegistry {
public:
    [[nodiscard]] static SanitizationMethodRegistry& instance();

    SanitizationMethodRegistry(const SanitizationMethodRegistry&) = delete;
    SanitizationMethodRegistry& operator=(const SanitizationMethodRegistry&) = delete;

    void add(DiskSanitizationInterface& method);

    [[nodiscard]] const std::vector<std::reference_wrapper<DiskSanitizationInterface>>& methods() const;

private:
    SanitizationMethodRegistry() = default;

    std::vector<std::reference_wrapper<DiskSanitizationInterface>> methods_;
};

template<typename T>
class AutoRegisterMethod {
protected:
    explicit AutoRegisterMethod(T& self) {
        SanitizationMethodRegistry::instance().add(self);
    }
};

#endif // SANITIZATION_METHOD_REGISTRY_HPP

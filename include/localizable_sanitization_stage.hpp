
#ifndef LOCALIZABLE_SANITIZATION_STAGE_HPP
#define LOCALIZABLE_SANITIZATION_STAGE_HPP

#include <string>

#include <sanitization_stage.hpp>

// Adds locale-aware rendering on top of the title()/description() keys. The
// no-argument variants resolve against the currently selected locale (see
// DiskManagement::setLocale); the locale overloads resolve against the given
// one.
class LocalizableSanitizationStage : public SanitizationStage {
public:

    using SanitizationStage::SanitizationStage;

    [[nodiscard]] std::string localizedTitle() const;
    [[nodiscard]] std::string localizedTitle(const std::string& locale) const;

    [[nodiscard]] std::string localizedDescription() const;
    [[nodiscard]] std::string localizedDescription(const std::string& locale) const;
};

#endif // LOCALIZABLE_SANITIZATION_STAGE_HPP

#include <localizable_sanitization_stage.hpp>

std::string LocalizableSanitizationStage::localizedTitle() const {
    return title_.localized();
}

std::string LocalizableSanitizationStage::localizedTitle(const std::string& locale) const {
    return title_.localized(locale);
}

std::string LocalizableSanitizationStage::localizedDescription() const {
    return description_.localized();
}

std::string LocalizableSanitizationStage::localizedDescription(const std::string& locale) const {
    return description_.localized(locale);
}


#ifndef SANITIZATION_STAGE_HPP
#define SANITIZATION_STAGE_HPP

#include <utility>

#include <localization/localization.hpp>

class SanitizationStage {

protected:
    Localization::LocalizedString title_;
    Localization::LocalizedString description_;

public:
    SanitizationStage(Localization::LocalizedString title, Localization::LocalizedString description)
        : title_(std::move(title)), description_(std::move(description)) {

    }
};

#endif // SANITIZATION_STAGE_HPP

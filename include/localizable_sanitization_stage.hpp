
#ifndef LOCALIZABLE_SANITIZATION_STAGE_HPP
#define LOCALIZABLE_SANITIZATION_STAGE_HPP

#include <sanitization_stage.hpp>

class LocalizableSanitizationStage : public SanitizationStage {
public:
    virtual std::string localizedTitle() const = 0;
    virtual std::string localizedDescription() const = 0;
};

#endif // LOCALIZABLE_SANITIZATION_STAGE_HPP

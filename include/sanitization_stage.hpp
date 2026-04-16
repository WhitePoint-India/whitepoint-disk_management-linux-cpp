
#ifndef SANITIZATION_STAGE_HPP
#define SANITIZATION_STAGE_HPP

#include <string>

class SanitizationStage {
public:
    virtual ~SanitizationStage() = default;
    virtual std::string title() const = 0;
    virtual std::string description() const = 0;
};

#endif // SANITIZATION_STAGE_HPP

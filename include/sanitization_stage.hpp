
#ifndef SANITIZATION_STAGE_HPP
#define SANITIZATION_STAGE_HPP

#include <string>
#include <functional>

class SanitizationStage {
public:
    virtual ~SanitizationStage() = default;
    virtual std::string title() const = 0;
    virtual std::string description() const = 0;
    virtual std::string localizedTitle() const = 0;
    virtual std::string localizedDescription() const = 0;
};

using SanitizationCallback = std::function<void(
    const SanitizationStage&,
    const int index,
    const int total
)>;

#endif // SANITIZATION_STAGE_HPP

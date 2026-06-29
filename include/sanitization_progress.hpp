
#ifndef SANITIZATION_PROGRESS_HPP
#define SANITIZATION_PROGRESS_HPP

#include <localizable_sanitization_stage.hpp>

class SanitizationProgress {
private:
    LocalizableSanitizationStage& stage_;
    int index_;
    int total_;
    double partialProgress_;
    double fractionCompleted_;
    double percentageCompleted_;
public:
    SanitizationProgress(
        LocalizableSanitizationStage& stage,
        int index,
        int total,
        double partialProgress = 1
    );
    [[nodiscard]] double fractionCompleted() const;
    [[nodiscard]] double percentageCompleted() const;
    [[nodiscard]] int currentIndex() const;
    [[nodiscard]] int totalStageCount() const;
    [[nodiscard]] const LocalizableSanitizationStage& getStage() const;
};

#endif // SANITIZATION_PROGRESS_HPP
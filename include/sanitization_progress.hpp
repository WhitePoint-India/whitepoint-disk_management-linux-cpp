
#ifndef SANITIZATION_PROGRESS_HPP
#define SANITIZATION_PROGRESS_HPP

#include <sanitization_stage.hpp>

class SanitizationProgress {
private:
    SanitizationStage& stage_;
    int index_;
    int total_;
    double partialProgress_;
    double fractionCompleted_;
    double percentageCompleted_;
public:
    SanitizationProgress(
        SanitizationStage& stage,
        int index,
        int total,
        double partialProgress
    );
    [[nodiscard]] double fractionCompleted() const;
    [[nodiscard]] double percentageCompleted() const;
    [[nodiscard]] int currentIndex() const;
    [[nodiscard]] int totalStageCount() const;
    [[nodiscard]] const SanitizationStage& getStage() const;
};

#endif // SANITIZATION_PROGRESS_HPP
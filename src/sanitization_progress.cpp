
#include <sanitization_progress.hpp>

SanitizationProgress::SanitizationProgress(
    SanitizationStage& stage,
    int index,
    int total,
    double partialProgress
) : stage_(stage), index_(index), total_(total), partialProgress_(partialProgress), fractionCompleted_(0), percentageCompleted_(0) {
    fractionCompleted_ = total_ > 0 ? (index_ + partialProgress_) / total_ : 1.0;
    percentageCompleted_ = fractionCompleted_ * 100.0;
}

double SanitizationProgress::fractionCompleted() const { return fractionCompleted_; }

double SanitizationProgress::percentageCompleted() const { return percentageCompleted_; }

int SanitizationProgress::currentIndex() const { return index_; }

int SanitizationProgress::totalStageCount() const { return total_; }

const SanitizationStage& SanitizationProgress::getStage() const { return stage_; }
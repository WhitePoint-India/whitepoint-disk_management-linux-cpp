#ifndef DISKS_TPP
#define DISKS_TPP

namespace DiskManagement {

template <typename Self, typename DeleteOperation>
void Deletable::deleteDisk(this Self& self, const DeleteOperation& operation, Callback callback) {
    int currentStage = 0;
    auto onStageChange = [&currentStage, &callback](auto stage) {
        int totalStages = static_cast<int>(decltype(stage)::COUNT);
        currentStage = static_cast<int>(stage);
        return callback(static_cast<double>(currentStage + 1) / totalStages);
    };
    auto onProgress = [&currentStage, &callback](auto stage, const Progress& progress) {
        int totalStages = static_cast<int>(decltype(stage)::COUNT);
        currentStage = static_cast<int>(stage);
        return callback(static_cast<double>(currentStage + progress.fractionCompleted()) / totalStages);
    };
    operation.deleteDisk(self, onStageChange, onProgress);
}

} // namespace DiskManagement

#endif // DISKS_TPP

#ifndef BLOCK_WRITING_METHOD
#define BLOCK_WRITING_METHOD

#include <vector>
#include <utility>
#include <functional>
#include <pattern.hpp>
#include <block_writable.hpp>
#include <localizable_sanitization_stage.hpp>

// Pure overwrite engine: an ordered list of passes, each writing one Pattern
// across the whole disk. Decoupled from sanitization/progress concerns — the
// DiskSanitizationInterface bridge (OverwriteMethod) maps its callback onto
// SanitizationProgress.
class BlockWritingMethod {

protected:

    class Pass: public LocalizableSanitizationStage {
    public:
        // Reports this pass's own completion fraction, in [0, 1].
        using Callback = std::function<void(double fractionCompleted)>;

        Pass(Pattern pattern);

        // Write this pass's pattern across the entire disk, then flush.
        void run(BlockWritable& writable, const Callback& onProgress) const;

    private:
        Pattern pattern_;
    };

    // Reports progress for pass `index` of `total`, at `fraction` within that pass.
    using Callback = std::function<void(const Pass& pass, int index, int total, double fraction)>;

    // Number of passes — used by bridges that prepend their own stages.
    [[nodiscard]] int passCount() const { return static_cast<int>(passes_.size()); }

private:
    std::vector<Pass> passes_;

public:

    template <typename... Passes>
    BlockWritingMethod(Passes&&... passes);

    void write(BlockWritable& writable, const Callback& callback);
};

#include <block_writing_method.tpp>

#endif // BLOCK_WRITING_METHOD
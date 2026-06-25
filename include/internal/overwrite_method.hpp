
#ifndef OVERWRITE_METHOD_HPP
#define OVERWRITE_METHOD_HPP

#include <string>
#include <vector>
#include <optional>

#include <localizable_sanitization_stage.hpp>
#include <disk_sanitization_interface.hpp>

// Shared base for every overwrite-based standard. A concrete method just supplies
// a key and an ordered list of passes; this base runs them via the BlockWritable
// overwrite engine (and the Verifiable engine for verification passes), mapping
// each pass to a SanitizationProgress.
class OverwriteMethod : public DiskSanitizationInterface {
public:
    void sanitize(Disk& disk, Callback callback) override;

    class Stage : public LocalizableSanitizationStage {
    public:
        Stage(std::string title, std::string description);
        std::string title() const override;
        std::string description() const override;
        std::string localizedTitle() const override;
        std::string localizedDescription() const override;
    private:
        std::string title_;
        std::string description_;
    };

protected:
    struct Pass {
        enum class Kind { Write, Verify };
        std::string label;
        Kind kind;
        // Write: nullopt => random data, else the (repeating) pattern to write.
        // Verify: nullopt => read-back integrity check, else expected pattern.
        std::optional<std::vector<unsigned char>> pattern;
        double sampling = 1.0;  // Verify only: 1.0 = full, 0.10 = 10% sample
    };

    OverwriteMethod(std::string key, std::vector<Pass> passes);

private:
    std::vector<Pass> passes_;
};

#endif // OVERWRITE_METHOD_HPP

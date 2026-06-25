
#include <overwrite_method.hpp>

#include <stdexcept>
#include <span>
#include <utility>

#include <disk.hpp>
#include <block_writable.hpp>
#include <verifiable.hpp>
#include <sanitization_progress.hpp>

OverwriteMethod::OverwriteMethod(std::string key, std::vector<Pass> passes)
    : DiskSanitizationInterface(std::move(key)), passes_(std::move(passes)) {
}

void OverwriteMethod::sanitize(Disk& disk, Callback callback) {
    auto* writable = dynamic_cast<BlockWritable*>(&disk);
    if (!writable) {
        throw std::invalid_argument(getKey() + " requires a block-writable disk");
    }
    auto* verifiable = dynamic_cast<Verifiable*>(&disk);

    const int total = static_cast<int>(passes_.size());
    for (int i = 0; i < total; ++i) {
        const Pass& pass = passes_[static_cast<std::size_t>(i)];
        Stage stage(pass.label, pass.label);

        const auto progress = [&](double fraction) {
            callback(SanitizationProgress(stage, i, total, fraction));
        };

        if (pass.kind == Pass::Kind::Write) {
            if (pass.pattern) {
                writable->overwrite(std::span<const unsigned char>(*pass.pattern), progress);
            } else {
                writable->overwrite(BlockWritable::Fill::Random, progress);
            }
            continue;
        }

        // Verify pass.
        if (!verifiable) {
            throw std::invalid_argument(getKey() + " requires a verifiable disk");
        }
        std::optional<std::span<const unsigned char>> expected;
        if (pass.pattern) {
            expected = std::span<const unsigned char>(*pass.pattern);
        }
        const Verifiable::VerificationResult result =
            (pass.sampling < 1.0)
                ? verifiable->verifySampling(pass.sampling, expected, progress)
                : verifiable->verify(expected, progress);
        if (!result.passed) {
            throw std::runtime_error(getKey() + ": verification failed (" +
                                     std::to_string(result.mismatches) + " mismatched sectors)");
        }
    }
}

OverwriteMethod::Stage::Stage(std::string title, std::string description)
    : title_(std::move(title)), description_(std::move(description)) {
}

std::string OverwriteMethod::Stage::title() const { return title_; }
std::string OverwriteMethod::Stage::description() const { return description_; }
std::string OverwriteMethod::Stage::localizedTitle() const { return title_; }
std::string OverwriteMethod::Stage::localizedDescription() const { return description_; }

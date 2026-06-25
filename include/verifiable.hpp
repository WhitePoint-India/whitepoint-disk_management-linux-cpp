
#ifndef VERIFIABLE_HPP
#define VERIFIABLE_HPP

#include <cstddef>
#include <cstdint>
#include <span>
#include <optional>
#include <functional>

class Verifiable {
public:
    using ProgressCallback = std::function<void(double fractionCompleted)>;

    struct VerificationResult {
        unsigned long long sectorsChecked;
        unsigned long long mismatches;
        bool passed;
    };

    virtual ~Verifiable() noexcept = default;

    // Primitive implemented by concrete disks.
    virtual void readBlock(uint64_t sectorOffset, void* data, std::size_t dataSize) = 0;

    // Declared identically to BlockWritable so a single override per disk is the
    // final overrider for both interfaces.
    [[nodiscard]] virtual unsigned long long getSectorCount() const noexcept = 0;
    [[nodiscard]] virtual unsigned int getSectorSize() const noexcept = 0;

    // Read back the whole disk and verify it. With `expected`, every sector must
    // equal the repeating pattern; with std::nullopt, this is a read-back
    // integrity check (all sectors readable). Reports progress.
    VerificationResult verify(std::optional<std::span<const unsigned char>> expected,
                              const ProgressCallback& onProgress);

    // As verify(), but only inspects a random `fraction` (0..1] of the disk.
    VerificationResult verifySampling(double fraction,
                                      std::optional<std::span<const unsigned char>> expected,
                                      const ProgressCallback& onProgress);
};

#endif // VERIFIABLE_HPP

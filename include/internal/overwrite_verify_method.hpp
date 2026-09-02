
#ifndef OVERWRITE_VERIFY_METHOD_HPP
#define OVERWRITE_VERIFY_METHOD_HPP

#include <string>
#include <utility>

#include <disk_sanitization_interface.hpp>
#include <block_writing_method.hpp>
#include <localizable_sanitization_stage.hpp>
#include <disk_management/localization>

// Bridges overwrite passes plus a final read-back verification to
// DiskSanitizationInterface: stages 0..N-1 are the overwrite passes, then
// stage N reads back 100% of the disk and requires every sector to be all
// 0x00 (every subclass ends with a zero pass; generalizing to other final
// patterns means parameterizing the expected pattern and the stage's
// description). Throws std::runtime_error if verification fails. Concrete
// standards inherit this and declare their passes inline, e.g.
//   OverwriteVerifyMethod("NIST_800", grade, Pass(Pattern::repeat(RepeatingByte::ZERO)))
class OverwriteVerifyMethod : public DiskSanitizationInterface, public BlockWritingMethod {
public:
    void sanitize(Disk& disk, DiskSanitizationInterface::Callback callback) override;

protected:
    template <typename... Passes>
    OverwriteVerifyMethod(std::string key, sanitization_grade grade, Passes&&... passes)
        : DiskSanitizationInterface(std::move(key), grade),
          BlockWritingMethod(std::forward<Passes>(passes)...) {}

private:
    // Stage N. A member so its lifetime matches the method singleton —
    // SanitizationProgress holds stages by non-owning reference, the same
    // contract as the Pass objects owned by BlockWritingMethod.
    LocalizableSanitizationStage verifyStage_{
        Localization::LocalizedString::key("verification.verify.title"),
        Localization::LocalizedString::key("verification.verify.desc")};
};

#endif // OVERWRITE_VERIFY_METHOD_HPP

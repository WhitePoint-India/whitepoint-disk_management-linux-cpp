
#ifndef OVERWRITE_METHOD_HPP
#define OVERWRITE_METHOD_HPP

#include <string>
#include <utility>

#include <disk_sanitization_interface.hpp>
#include <block_writing_method.hpp>

// Bridges the BlockWritingMethod overwrite engine to DiskSanitizationInterface:
// it supplies the method key and maps each pass's progress onto a
// SanitizationProgress. Concrete standards inherit this and declare their passes
// inline, e.g.
//   OverwriteMethod("ZERO_FILL", Pass(Pattern::repeat(RepeatingByte::ZERO)))
class OverwriteMethod : public DiskSanitizationInterface, public BlockWritingMethod {
public:
    void sanitize(Disk& disk, DiskSanitizationInterface::Callback callback) override;

protected:
    template <typename... Passes>
    OverwriteMethod(std::string key, Passes&&... passes)
        : DiskSanitizationInterface(std::move(key)),
          BlockWritingMethod(std::forward<Passes>(passes)...) {}
};

#endif // OVERWRITE_METHOD_HPP

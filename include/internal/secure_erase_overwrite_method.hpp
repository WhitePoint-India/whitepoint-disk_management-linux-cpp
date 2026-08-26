
#ifndef SECURE_ERASE_OVERWRITE_METHOD_HPP
#define SECURE_ERASE_OVERWRITE_METHOD_HPP

#include <string>
#include <utility>

#include <disk_sanitization_interface.hpp>
#include <block_writing_method.hpp>

// Bridges a secure erase followed by overwrite passes to DiskSanitizationInterface.
// sanitize() reports a unified stage sequence: stage 0 is the secure erase
// (delegated to the SecureErase method, which dispatches ATA vs NVMe), then stages
// 1..N are the overwrite passes from the BlockWritingMethod engine. Concrete
// standards inherit this and declare their passes inline, e.g.
//   SecureEraseOverwriteMethod("NIST_800", Pass(Pattern::repeat(RepeatingByte::ZERO)))
class SecureEraseOverwriteMethod : public DiskSanitizationInterface, public BlockWritingMethod {
public:
    void sanitize(Disk& disk, DiskSanitizationInterface::Callback callback) override;

protected:
    template <typename... Passes>
    SecureEraseOverwriteMethod(std::string key, sanitization_grade grade, Passes&&... passes)
        : DiskSanitizationInterface(std::move(key), grade),
          BlockWritingMethod(std::forward<Passes>(passes)...) {}
};

#endif // SECURE_ERASE_OVERWRITE_METHOD_HPP

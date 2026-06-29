

#include <block_writable.hpp>
#include <block_writing_method.hpp>

void BlockWritingMethod::write(BlockWritable& writable, const Callback& callback) {
    const int total = static_cast<int>(passes_.size());
    for (int i = 0; i < total; ++i) {
        const Pass& pass = passes_[static_cast<std::size_t>(i)];
        pass.run(writable, [&](double fraction) {
            callback(pass, i, total, fraction);
        });
    }
}
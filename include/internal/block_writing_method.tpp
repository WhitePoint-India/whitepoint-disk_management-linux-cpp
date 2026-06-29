
// Included at the bottom of <block_writing_method.hpp>; not a standalone TU.

template <typename... Passes>
BlockWritingMethod::BlockWritingMethod(Passes&&... passes) : passes_{ std::forward<Passes>(passes)... } {

}

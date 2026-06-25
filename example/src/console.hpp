#ifndef EXAMPLE_CONSOLE_HPP
#define EXAMPLE_CONSOLE_HPP

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

// Generic, library-agnostic terminal helpers for the example CLI.
namespace console {

// Clear the screen and move the cursor home.
void clearScreen();

// Print a framed page title.
void header(const std::string& title);

// Prompt for a single 1-based selection in [1, count]. Re-prompts on invalid
// input. Returns a 0-based index, or std::nullopt if the user quits ("q") or
// input ends (EOF).
[[nodiscard]] std::optional<std::size_t> promptIndex(std::size_t count, const std::string& label);

// Prompt for one or more selections, accepting comma/space-separated indices
// or "all". Re-prompts on invalid input. Returns sorted, de-duplicated 0-based
// indices, or std::nullopt on quit/EOF.
[[nodiscard]] std::optional<std::vector<std::size_t>> promptMultiSelect(std::size_t count);

// Returns true only if the user types exactly "yes".
[[nodiscard]] bool confirmYes(const std::string& prompt);

}  // namespace console

#endif  // EXAMPLE_CONSOLE_HPP

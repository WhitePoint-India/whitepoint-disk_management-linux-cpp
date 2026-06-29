

#ifndef PATTERN_HPP
#define PATTERN_HPP

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <concepts>
#include <repeating_byte.hpp>
#include <localization/localization.hpp>

class Pattern {

    private:
    std::variant<std::vector<unsigned char>, RepeatingByte> pattern_;

    Pattern(std::variant<std::vector<unsigned char>, RepeatingByte> pattern);

    // The argument that fills the {0} slot of the writing.pattern.@ template: a
    // localizable word for repeating bytes, or the literal hex for a sequence.
    [[nodiscard]] Localization::LocalizedString valueArg() const;

    // The byte sequence formatted as "0x" + concatenated uppercase hex,
    // e.g. {0x7F, 0xFF, 0xFF, 0xFF} -> "0x7FFFFFFF".
    [[nodiscard]] std::string hex() const;

    public:
    static Pattern random();

    static Pattern repeat(RepeatingByte byte);

    [[nodiscard]] Localization::LocalizedString title() const {
        return Localization::LocalizedString::key("writing.pattern.@.title", { valueArg() });
    }

    [[nodiscard]] Localization::LocalizedString description() const {
        return Localization::LocalizedString::key("writing.pattern.@.description", { valueArg() });
    }

    // The repeating byte sequence to write, or nullopt for random data.
    // ZERO -> {0x00}, ONE -> {0xFF}, a sequence -> its bytes, RANDOM -> nullopt.
    [[nodiscard]] std::optional<std::vector<unsigned char>> repeatingBytes() const;

    template <std::integral... Bytes>
    static Pattern sequence(Bytes... bytes);
};

#include <pattern.tpp>

#endif // PATTERN_HPP
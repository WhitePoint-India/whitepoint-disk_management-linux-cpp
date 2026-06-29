

#include <utility>
#include <string>
#include <pattern.hpp>

Pattern::Pattern(std::variant<std::vector<unsigned char>, RepeatingByte> pattern)
    : pattern_(std::move(pattern)) {

}

Pattern Pattern::random() {
    return Pattern(RepeatingByte::RANDOM);
}

Pattern Pattern::repeat(RepeatingByte byte) {
    return Pattern(byte);
}

std::optional<std::vector<unsigned char>> Pattern::repeatingBytes() const {
    if (const auto* bytes = std::get_if<std::vector<unsigned char>>(&pattern_)) {
        return *bytes;
    }
    switch (std::get<RepeatingByte>(pattern_)) {
        case RepeatingByte::ZERO: return std::vector<unsigned char>{ 0x00 };
        case RepeatingByte::ONE:  return std::vector<unsigned char>{ 0xFF };
        case RepeatingByte::RANDOM: return std::nullopt;
    }
    return std::nullopt;
}

std::string Pattern::hex() const {
    const auto* bytes = std::get_if<std::vector<unsigned char>>(&pattern_);
    if (!bytes || bytes->empty()) {
        return "0x00";
    }
    static constexpr char digits[] = "0123456789ABCDEF";
    std::string out = "0x";
    out.reserve(2 + bytes->size() * 2);
    for (const unsigned char b : *bytes) {
        out.push_back(digits[b >> 4]);
        out.push_back(digits[b & 0x0F]);
    }
    return out;
}

Localization::LocalizedString Pattern::valueArg() const {
    if (std::holds_alternative<std::vector<unsigned char>>(pattern_)) {
        return Localization::LocalizedString::literal(hex());
    }
    switch (std::get<RepeatingByte>(pattern_)) {
        case RepeatingByte::ZERO:   return Localization::LocalizedString::key("writing.pattern.value.zero");
        case RepeatingByte::ONE:    return Localization::LocalizedString::key("writing.pattern.value.one");
        case RepeatingByte::RANDOM: return Localization::LocalizedString::key("writing.pattern.value.random");
    }
    return Localization::LocalizedString::key("writing.pattern.value.random");
}

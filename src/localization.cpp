#include <disk_management/localization>

#include <optional>
#include <unordered_map>

namespace Localization {
namespace {

using Catalog = std::unordered_map<std::string, std::string>;

// Process-wide state. A library consumed via static linking has a single copy
// of these, which is exactly the intent: one global locale selection.
struct State {
    std::unordered_map<std::string, Catalog> catalogs;  // locale -> (key -> value)
    std::string current = "en";
    std::string fallback = "en";
};

State& state() {
    static State instance;
    return instance;
}

std::string_view trim(std::string_view text) {
    const auto isSpace = [](char c) {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' || c == '\v';
    };
    while (!text.empty() && isSpace(text.front())) {
        text.remove_prefix(1);
    }
    while (!text.empty() && isSpace(text.back())) {
        text.remove_suffix(1);
    }
    return text;
}

// Look up `key` in the catalog for `localeCode`, if any.
std::optional<std::string> lookup(const std::string& localeCode, const std::string& key) {
    const auto catalogIt = state().catalogs.find(localeCode);
    if (catalogIt == state().catalogs.end()) {
        return std::nullopt;
    }
    const auto entryIt = catalogIt->second.find(key);
    if (entryIt == catalogIt->second.end()) {
        return std::nullopt;
    }
    return entryIt->second;
}

// Resolve `key` against `primary`, then the default locale.
std::optional<std::string> resolve(const std::string& primary, const std::string& key) {
    if (auto value = lookup(primary, key)) {
        return value;
    }
    if (state().fallback != primary) {
        if (auto value = lookup(state().fallback, key)) {
            return value;
        }
    }
    return std::nullopt;
}

}  // namespace

void setLocale(std::string code) {
    state().current = std::move(code);
}

std::string locale() {
    return state().current;
}

void setDefaultLocale(std::string code) {
    state().fallback = std::move(code);
}

void registerCatalog(const std::string& localeCode, std::string_view keyValueText) {
    Catalog& catalog = state().catalogs[localeCode];

    std::size_t pos = 0;
    while (pos <= keyValueText.size()) {
        const std::size_t newline = keyValueText.find('\n', pos);
        const std::size_t end = (newline == std::string_view::npos) ? keyValueText.size() : newline;
        const std::string_view rawLine = keyValueText.substr(pos, end - pos);
        pos = end + 1;

        const std::string_view line = trim(rawLine);
        if (line.empty() || line.front() == '#') {
            if (newline == std::string_view::npos) {
                break;
            }
            continue;
        }

        const std::size_t eq = line.find('=');
        if (eq != std::string_view::npos) {
            const std::string_view key = trim(line.substr(0, eq));
            const std::string_view value = trim(line.substr(eq + 1));
            if (!key.empty()) {
                catalog[std::string(key)] = std::string(value);
            }
        }

        if (newline == std::string_view::npos) {
            break;
        }
    }
}

namespace {

// Replace `{n}` slots in `value` with the rendering of `args[n]` in `localeCode`.
// Out-of-range indices are left as the literal `{n}`. `{{` and `}}` emit a single
// brace so values can contain literal braces.
std::string interpolate(const std::string& value, const std::string& localeCode,
                        const std::vector<LocalizedString>& args) {
    if (args.empty() && value.find('{') == std::string::npos) {
        return value;
    }

    std::string out;
    out.reserve(value.size());
    for (std::size_t i = 0; i < value.size();) {
        const char c = value[i];
        if (c == '{') {
            if (i + 1 < value.size() && value[i + 1] == '{') {
                out.push_back('{');
                i += 2;
                continue;
            }
            const std::size_t close = value.find('}', i + 1);
            if (close != std::string::npos) {
                const std::string_view digits(value.data() + i + 1, close - i - 1);
                bool allDigits = !digits.empty();
                std::size_t index = 0;
                for (const char d : digits) {
                    if (d < '0' || d > '9') { allDigits = false; break; }
                    index = index * 10 + static_cast<std::size_t>(d - '0');
                }
                if (allDigits && index < args.size()) {
                    out += args[index].localized(localeCode);
                    i = close + 1;
                    continue;
                }
            }
            // Not a valid placeholder — emit the brace literally.
            out.push_back('{');
            ++i;
            continue;
        }
        if (c == '}' && i + 1 < value.size() && value[i + 1] == '}') {
            out.push_back('}');
            i += 2;
            continue;
        }
        out.push_back(c);
        ++i;
    }
    return out;
}

}  // namespace

LocalizedString LocalizedString::literal(std::string text) {
    LocalizedString s;
    s.literal_ = true;
    s.text_ = std::move(text);
    return s;
}

LocalizedString LocalizedString::key(std::string key) {
    LocalizedString s;
    s.literal_ = false;
    s.text_ = std::move(key);
    return s;
}

LocalizedString LocalizedString::key(std::string key, std::vector<LocalizedString> args) {
    LocalizedString s;
    s.literal_ = false;
    s.text_ = std::move(key);
    s.args_ = std::move(args);
    return s;
}

std::string LocalizedString::localized() const {
    return localized(state().current);
}

std::string LocalizedString::localized(const std::string& localeCode) const {
    if (literal_) {
        return text_;
    }
    const std::string value = resolve(localeCode, text_).value_or(text_);
    return interpolate(value, localeCode, args_);
}

}  // namespace Localization

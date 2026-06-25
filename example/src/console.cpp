#include "console.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace console {
namespace {

// Read one line from stdin. Returns false on EOF.
bool readLine(std::string& out) {
    return static_cast<bool>(std::getline(std::cin, out));
}

std::string trim(const std::string& s) {
    const auto begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

bool isQuit(const std::string& s) {
    return s == "q" || s == "Q";
}

}  // namespace

void clearScreen() {
    std::cout << "\033[2J\033[H";
}

void header(const std::string& title) {
    std::cout << "==================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "==================================================\n\n";
}

std::optional<std::size_t> promptIndex(std::size_t count, const std::string& label) {
    while (true) {
        std::cout << label << " (1-" << count << ", q to quit): " << std::flush;
        std::string line;
        if (!readLine(line)) {
            return std::nullopt;
        }
        line = trim(line);
        if (isQuit(line)) {
            return std::nullopt;
        }
        try {
            const std::size_t choice = std::stoul(line);
            if (choice >= 1 && choice <= count) {
                return choice - 1;
            }
        } catch (const std::exception&) {
            // fall through to the error message
        }
        std::cout << "  Invalid selection. Please try again.\n";
    }
}

std::optional<std::vector<std::size_t>> promptMultiSelect(std::size_t count) {
    while (true) {
        std::cout << "Select (e.g. \"1,3 4\" or \"all\", q to quit): " << std::flush;
        std::string line;
        if (!readLine(line)) {
            return std::nullopt;
        }
        line = trim(line);
        if (isQuit(line)) {
            return std::nullopt;
        }
        if (line == "all" || line == "ALL") {
            std::vector<std::size_t> all(count);
            for (std::size_t i = 0; i < count; ++i) {
                all[i] = i;
            }
            return all;
        }

        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream stream(line);
        std::vector<std::size_t> indices;
        bool valid = !line.empty();
        std::string token;
        while (stream >> token) {
            try {
                const std::size_t choice = std::stoul(token);
                if (choice >= 1 && choice <= count) {
                    indices.push_back(choice - 1);
                } else {
                    valid = false;
                    break;
                }
            } catch (const std::exception&) {
                valid = false;
                break;
            }
        }
        if (valid && !indices.empty()) {
            std::sort(indices.begin(), indices.end());
            indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
            return indices;
        }
        std::cout << "  Invalid selection. Please try again.\n";
    }
}

bool confirmYes(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string line;
    if (!readLine(line)) {
        return false;
    }
    return trim(line) == "yes";
}

}  // namespace console

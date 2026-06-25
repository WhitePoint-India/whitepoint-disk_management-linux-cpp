
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <optional>

#include <disk_management>

namespace {

void clearScreen() {
    std::cout << "\033[2J\033[H";
}

void header(const std::string& title) {
    std::cout << "==================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "==================================================\n\n";
}

// Read a non-empty line from stdin. Returns false on EOF.
bool readLine(std::string& line) {
    if (!std::getline(std::cin, line)) {
        return false;
    }
    return true;
}

std::string trim(const std::string& s) {
    const auto begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

// Prompt for a single 1-based index in [1, count]. Re-prompts until valid.
// Returns a 0-based index, or std::nullopt on EOF/quit.
std::optional<std::size_t> promptIndex(std::size_t count, const std::string& prompt) {
    while (true) {
        std::cout << prompt << " (1-" << count << ", q to quit): " << std::flush;
        std::string line;
        if (!readLine(line)) {
            return std::nullopt;
        }
        line = trim(line);
        if (line == "q" || line == "Q") {
            return std::nullopt;
        }
        try {
            const std::size_t choice = std::stoul(line);
            if (choice >= 1 && choice <= count) {
                return choice - 1;
            }
        } catch (const std::exception&) {
            // fall through to error message
        }
        std::cout << "  Invalid selection. Please try again.\n";
    }
}

// Parse a multi-select line: comma/space-separated 1-based indices, or "all".
// Returns sorted, de-duplicated 0-based indices. Re-prompts until valid.
// Returns std::nullopt on EOF/quit.
std::optional<std::vector<std::size_t>> promptMultiSelect(std::size_t count) {
    while (true) {
        std::cout << "Select disks (e.g. \"1,3 4\" or \"all\", q to quit): " << std::flush;
        std::string line;
        if (!readLine(line)) {
            return std::nullopt;
        }
        line = trim(line);
        if (line == "q" || line == "Q") {
            return std::nullopt;
        }
        if (line == "all" || line == "ALL") {
            std::vector<std::size_t> all(count);
            for (std::size_t i = 0; i < count; ++i) {
                all[i] = i;
            }
            return all;
        }

        // Replace commas with spaces, then tokenize.
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

// Require the user to type "yes" to proceed. Anything else cancels.
bool confirmYes() {
    std::cout << "\nType 'yes' to PERMANENTLY ERASE the disks above, anything else to cancel: " << std::flush;
    std::string line;
    if (!readLine(line)) {
        return false;
    }
    return trim(line) == "yes";
}

std::string sizeInGB(unsigned long long bytes) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2)
        << static_cast<double>(bytes) / (1000.0 * 1000.0 * 1000.0) << " GB";
    return out.str();
}

void renderProgress(const std::string& path, const SanitizationProgress& progress) {
    std::ostringstream out;
    out << "\r  [" << path << "] "
        << "[" << (progress.currentIndex() + 1) << "/" << progress.totalStageCount() << " "
        << progress.getStage().title() << "] "
        << std::fixed << std::setprecision(1) << progress.fractionCompleted() * 100.0 << "%"
        << "          ";  // trailing pad to overwrite shorter previous lines
    std::cout << out.str() << std::flush;
}

struct Result {
    std::string path;
    bool ok;
    std::string error;
};

}  // namespace

int main() {
    // ---- Page 1: choose a sanitization method --------------------------------
    const auto& methods = DiskManagement::methods();
    if (methods.empty()) {
        std::cerr << "No sanitization methods are registered." << std::endl;
        return 1;
    }

    clearScreen();
    header("Select a disk deletion method");
    for (std::size_t i = 0; i < methods.size(); ++i) {
        std::cout << "  " << std::setw(2) << (i + 1) << ". " << methods[i].get().getKey() << "\n";
    }
    std::cout << "\n";

    const auto methodChoice = promptIndex(methods.size(), "Method");
    if (!methodChoice) {
        std::cout << "Cancelled.\n";
        return 0;
    }
    DiskSanitizationInterface& method = methods[*methodChoice].get();

    // ---- Page 2: choose target disks -----------------------------------------
    clearScreen();
    header("Method: " + method.getKey() + "  -  Select target disk(s)");

    std::cout << "Scanning for disks..." << std::endl;
    auto disks = DiskManagement::fetchDisks();
    if (disks.empty()) {
        std::cout << "No disks found.\n";
        return 0;
    }

    clearScreen();
    header("Method: " + method.getKey() + "  -  Select target disk(s)");
    for (std::size_t i = 0; i < disks.size(); ++i) {
        const auto& disk = disks[i];
        std::cout << "  " << std::setw(2) << (i + 1) << ". " << disk->getPath()
                  << "  |  " << disk->getModel()
                  << "  |  " << sizeInGB(disk->getSize()) << "\n";
    }
    std::cout << "\n";

    const auto diskChoices = promptMultiSelect(disks.size());
    if (!diskChoices) {
        std::cout << "Cancelled.\n";
        return 0;
    }

    // ---- Confirmation (destructive gate) -------------------------------------
    clearScreen();
    header("Confirm destruction");
    std::cout << "Method : " << method.getKey() << "\n";
    std::cout << "Disks  :\n";
    for (const std::size_t index : *diskChoices) {
        std::cout << "    - " << disks[index]->getPath()
                  << "  (" << disks[index]->getModel() << ", " << sizeInGB(disks[index]->getSize()) << ")\n";
    }
    std::cout << "\nWARNING: this PERMANENTLY and IRREVERSIBLY destroys all data on the disks above.\n";

    if (!confirmYes()) {
        std::cout << "\nCancelled. No data was modified.\n";
        return 0;
    }

    // ---- Page 3: sanitize sequentially ---------------------------------------
    clearScreen();
    header("Erasing " + std::to_string(diskChoices->size()) + " disk(s) with " + method.getKey());

    std::vector<Result> results;
    for (const std::size_t index : *diskChoices) {
        Disk& disk = *disks[index];
        const std::string path = disk.getPath();
        std::cout << "Disk: " << path << " (" << disk.getModel() << ")\n";
        try {
            method.sanitize(disk, [&path](const SanitizationProgress progress) {
                renderProgress(path, progress);
            });
            std::cout << "\n  Done.\n\n";
            results.push_back({path, true, ""});
        } catch (const std::exception& e) {
            std::cout << "\n  FAILED: " << e.what() << "\n\n";
            results.push_back({path, false, e.what()});
        }
    }

    // ---- Page 4: completion summary ------------------------------------------
    clearScreen();
    header("Completed");

    std::size_t succeeded = 0;
    for (const auto& result : results) {
        if (result.ok) {
            std::cout << "  [OK]   " << result.path << "\n";
            ++succeeded;
        } else {
            std::cout << "  [FAIL] " << result.path << "  -  " << result.error << "\n";
        }
    }
    std::cout << "\n" << succeeded << " succeeded, " << (results.size() - succeeded) << " failed.\n";

    return results.size() == succeeded ? 0 : 1;
}

#include "app.hpp"

#include "console.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <disk_management>

namespace app {
namespace {

// Outcome of erasing a single disk.
struct Result {
    std::string path;
    bool ok;
    std::string error;
};

std::string humanSize(unsigned long long bytes) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2)
        << static_cast<double>(bytes) / (1000.0 * 1000.0 * 1000.0) << " GB";
    return out.str();
}

// In-place progress line for one disk; the caller prints a newline when done.
void renderProgress(const std::string& path, const SanitizationProgress& progress) {
    std::ostringstream out;
    out << "\r  [" << path << "] "
        << "[" << (progress.currentIndex() + 1) << "/" << progress.totalStageCount() << " "
        << progress.getStage().title() << "] "
        << std::fixed << std::setprecision(1) << progress.fractionCompleted() * 100.0 << "%"
        << "          ";  // pad to overwrite any longer previous line
    std::cout << out.str() << std::flush;
}

// Page 1: choose a sanitization method. Returns nullptr if cancelled.
DiskSanitizationInterface* selectMethod() {
    const auto& methods = DiskManagement::methods();

    console::clearScreen();
    console::header("Select a disk deletion method");
    for (std::size_t i = 0; i < methods.size(); ++i) {
        std::cout << "  " << std::setw(2) << (i + 1) << ". " << methods[i].get().getKey() << "\n";
    }
    std::cout << "\n";

    const auto choice = console::promptIndex(methods.size(), "Method");
    if (!choice) {
        return nullptr;
    }
    return &methods[*choice].get();
}

// Page 2: choose target disks. Returns nullopt if cancelled.
std::optional<std::vector<Disk*>> selectDisks(const std::vector<std::unique_ptr<Disk>>& disks,
                                              const std::string& methodKey) {
    console::clearScreen();
    console::header("Method: " + methodKey + "  -  Select target disk(s)");
    for (std::size_t i = 0; i < disks.size(); ++i) {
        const Disk& disk = *disks[i];
        std::cout << "  " << std::setw(2) << (i + 1) << ". " << disk.getPath()
                  << "  |  " << disk.getModel()
                  << "  |  " << humanSize(disk.getSize()) << "\n";
    }
    std::cout << "\n";

    const auto choices = console::promptMultiSelect(disks.size());
    if (!choices) {
        return std::nullopt;
    }
    std::vector<Disk*> targets;
    targets.reserve(choices->size());
    for (const std::size_t index : *choices) {
        targets.push_back(disks[index].get());
    }
    return targets;
}

// Destructive confirmation gate.
bool confirmDestruction(const std::string& methodKey, const std::vector<Disk*>& targets) {
    console::clearScreen();
    console::header("Confirm destruction");
    std::cout << "Method : " << methodKey << "\n";
    std::cout << "Disks  :\n";
    for (const Disk* disk : targets) {
        std::cout << "    - " << disk->getPath()
                  << "  (" << disk->getModel() << ", " << humanSize(disk->getSize()) << ")\n";
    }
    std::cout << "\nWARNING: this PERMANENTLY and IRREVERSIBLY destroys all data on the disks above.\n";
    return console::confirmYes("\nType 'yes' to erase, anything else to cancel: ");
}

// Page 3: erase each disk sequentially, reporting progress.
std::vector<Result> eraseAll(DiskSanitizationInterface& method, const std::vector<Disk*>& targets) {
    console::clearScreen();
    console::header("Erasing " + std::to_string(targets.size()) + " disk(s) with " + method.getKey());

    std::vector<Result> results;
    results.reserve(targets.size());
    for (Disk* disk : targets) {
        const std::string path = disk->getPath();
        std::cout << "Disk: " << path << " (" << disk->getModel() << ")\n";
        try {
            method.sanitize(*disk, [&path](const SanitizationProgress progress) {
                renderProgress(path, progress);
            });
            std::cout << "\n  Done.\n\n";
            results.push_back({path, true, ""});
        } catch (const std::exception& e) {
            std::cout << "\n  FAILED: " << e.what() << "\n\n";
            results.push_back({path, false, e.what()});
        }
    }
    return results;
}

// Page 4: completion summary.
void showSummary(const std::vector<Result>& results) {
    console::clearScreen();
    console::header("Completed");

    std::size_t succeeded = 0;
    for (const Result& result : results) {
        if (result.ok) {
            std::cout << "  [OK]   " << result.path << "\n";
            ++succeeded;
        } else {
            std::cout << "  [FAIL] " << result.path << "  -  " << result.error << "\n";
        }
    }
    std::cout << "\n" << succeeded << " succeeded, " << (results.size() - succeeded) << " failed.\n";
}

}  // namespace

int run() {
    if (DiskManagement::methods().empty()) {
        std::cerr << "No sanitization methods are registered." << std::endl;
        return 1;
    }

    DiskSanitizationInterface* method = selectMethod();
    if (method == nullptr) {
        std::cout << "Cancelled.\n";
        return 0;
    }

    std::cout << "Scanning for disks..." << std::endl;
    auto disks = DiskManagement::fetchDisks();
    if (disks.empty()) {
        std::cout << "No disks found.\n";
        return 0;
    }

    const auto targets = selectDisks(disks, method->getKey());
    if (!targets) {
        std::cout << "Cancelled.\n";
        return 0;
    }

    if (!confirmDestruction(method->getKey(), *targets)) {
        std::cout << "\nCancelled. No data was modified.\n";
        return 0;
    }

    const std::vector<Result> results = eraseAll(*method, *targets);
    showSummary(results);

    const bool allOk = std::all_of(results.begin(), results.end(),
                                   [](const Result& r) { return r.ok; });
    return allOk ? 0 : 1;
}

}  // namespace app

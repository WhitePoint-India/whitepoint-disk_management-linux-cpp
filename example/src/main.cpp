
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <termios.h>
#include <unistd.h>

#include <disk_management>

// ─── ANSI helpers ────────────────────────────────────────────────────────────

namespace ansi {
    constexpr const char* reset   = "\033[0m";
    constexpr const char* bold    = "\033[1m";
    constexpr const char* dim     = "\033[2m";
    constexpr const char* red     = "\033[31m";
    constexpr const char* green   = "\033[32m";
    constexpr const char* yellow  = "\033[33m";
    constexpr const char* blue    = "\033[34m";
    constexpr const char* magenta = "\033[35m";
    constexpr const char* cyan    = "\033[36m";
    constexpr const char* white   = "\033[37m";

    void clearScreen() { std::cout << "\033[2J\033[H" << std::flush; }
    void moveTo(int row, int col) { std::cout << "\033[" << row << ";" << col << "H" << std::flush; }
    void hideCursor() { std::cout << "\033[?25l" << std::flush; }
    void showCursor() { std::cout << "\033[?25h" << std::flush; }
    void clearLine()  { std::cout << "\033[2K" << std::flush; }
}

// ─── Raw terminal input ─────────────────────────────────────────────────────

enum class Key { Up, Down, Enter, Space, Quit, Other };

class RawMode {
    termios original_{};
public:
    RawMode() {
        tcgetattr(STDIN_FILENO, &original_);
        termios raw = original_;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }
    ~RawMode() { tcsetattr(STDIN_FILENO, TCSANOW, &original_); }

    static Key readKey() {
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1) return Key::Other;
        if (c == '\n' || c == '\r') return Key::Enter;
        if (c == ' ')  return Key::Space;
        if (c == 'q' || c == 'Q') return Key::Quit;
        if (c == '\033') {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) != 1) return Key::Other;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) return Key::Other;
            if (seq[0] == '[') {
                if (seq[1] == 'A') return Key::Up;
                if (seq[1] == 'B') return Key::Down;
            }
        }
        return Key::Other;
    }
};

// ─── Utilities ───────────────────────────────────────────────────────────────

static std::string formatBytes(unsigned long long bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    double v = static_cast<double>(bytes);
    while (v >= 1024.0 && i < 4) { v /= 1024.0; i++; }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(i == 0 ? 0 : 1) << v << " " << units[i];
    return oss.str();
}

static std::string diskTypeLabel(const DiskVariant& disk) {
    return std::visit([](const auto& d) -> std::string {
        using T = std::decay_t<decltype(d)>;
        if constexpr (std::is_same_v<T, Disks::ATADisk>)  return "ATA";
        if constexpr (std::is_same_v<T, Disks::NVMeDisk>) return "NVMe";
        if constexpr (std::is_same_v<T, Disks::USBDisk>)  return "USB";
    }, disk);
}

static std::string diskTypeColor(const DiskVariant& disk) {
    return std::visit([](const auto& d) -> std::string {
        using T = std::decay_t<decltype(d)>;
        if constexpr (std::is_same_v<T, Disks::ATADisk>)  return ansi::yellow;
        if constexpr (std::is_same_v<T, Disks::NVMeDisk>) return ansi::cyan;
        if constexpr (std::is_same_v<T, Disks::USBDisk>)  return ansi::magenta;
    }, disk);
}

static Disk& diskRef(DiskVariant& v) {
    return std::visit([](auto& d) -> Disk& { return d; }, v);
}

static void printHeader(const std::string& subtitle = "") {
    std::cout << ansi::bold << ansi::cyan
              << "  ╔══════════════════════════════════════╗\n"
              << "  ║       Disk Sanitization Tool         ║\n"
              << "  ╚══════════════════════════════════════╝"
              << ansi::reset << "\n";
    if (!subtitle.empty()) {
        std::cout << ansi::dim << "  " << subtitle << ansi::reset << "\n";
    }
    std::cout << "\n";
}

// ─── Screen 1: Method Selection ──────────────────────────────────────────────

static int selectMethod() {
    auto& methods = DiskManagement::methods;
    int cursor = 0;
    int total = static_cast<int>(methods.size());

    RawMode raw;
    ansi::hideCursor();

    auto draw = [&] {
        ansi::clearScreen();
        printHeader("Select a delete method");

        for (int i = 0; i < total; i++) {
            if (i == cursor) {
                std::cout << ansi::bold << ansi::green << "  ▸ " << ansi::reset
                          << ansi::bold << methods[i]->getTitle() << ansi::reset << "\n";
            } else {
                std::cout << ansi::dim << "    " << methods[i]->getTitle() << ansi::reset << "\n";
            }
        }

        // Detail panel
        std::cout << "\n"
                  << ansi::bold << ansi::blue << "  ── " << methods[cursor]->getTitle() << " ──" << ansi::reset << "\n"
                  << ansi::dim << "  " << methods[cursor]->getDescription() << ansi::reset << "\n\n"
                  << ansi::bold << "  Stages:" << ansi::reset << "\n";

        auto stages = methods[cursor]->getStages();
        for (const auto& s : stages) {
            std::cout << ansi::cyan << "    " << (s.index + 1) << ". " << ansi::reset << s.name << "\n";
        }

        std::cout << "\n" << ansi::dim << "  ↑↓ navigate  ⏎ select  q quit" << ansi::reset << std::flush;
    };

    draw();
    while (true) {
        Key key = RawMode::readKey();
        if (key == Key::Up)    { cursor = (cursor - 1 + total) % total; draw(); }
        if (key == Key::Down)  { cursor = (cursor + 1) % total; draw(); }
        if (key == Key::Enter) { ansi::showCursor(); return cursor; }
        if (key == Key::Quit)  { ansi::showCursor(); return -1; }
    }
}

// ─── Screen 2: Disk Checkbox Selection ───────────────────────────────────────

static std::vector<int> selectDisks(
    std::vector<DiskVariant>& disks,
    const std::string& methodTitle
) {
    if (disks.empty()) {
        ansi::clearScreen();
        printHeader();
        std::cout << ansi::bold << ansi::red
                  << "  No disks found. Make sure you are running as root (sudo).\n"
                  << ansi::reset;
        ansi::showCursor();
        return {};
    }

    int cursor = 0;
    int total = static_cast<int>(disks.size());
    std::vector<bool> checked(total, false);

    RawMode raw;
    ansi::hideCursor();

    auto draw = [&] {
        ansi::clearScreen();
        printHeader("Method: " + methodTitle);

        std::cout << ansi::bold << "  Select disks to sanitize:\n\n" << ansi::reset;

        for (int i = 0; i < total; i++) {
            auto& d = diskRef(disks[i]);
            std::string typeCol = diskTypeColor(disks[i]);

            // Checkbox
            std::string box = checked[i]
                ? (std::string(ansi::green) + ansi::bold + "[✓]" + ansi::reset)
                : (std::string(ansi::dim) + "[ ]" + ansi::reset);

            // Cursor marker
            std::string marker = (i == cursor)
                ? (std::string(ansi::bold) + ansi::green + " ▸ " + ansi::reset)
                : "   ";

            std::cout << marker << box << " "
                      << ansi::bold << d.getPath() << ansi::reset
                      << "  " << d.getModel()
                      << "  " << typeCol << "[" << diskTypeLabel(disks[i]) << "]" << ansi::reset
                      << ansi::dim << "  " << formatBytes(d.getSize()) << ansi::reset
                      << "\n";
        }

        int count = 0;
        for (bool c : checked) if (c) count++;

        std::cout << "\n" << ansi::dim << "  " << count << " disk(s) selected" << ansi::reset << "\n";
        std::cout << "\n" << ansi::dim << "  ↑↓ navigate  ␣ toggle  ⏎ confirm  q quit" << ansi::reset << std::flush;
    };

    draw();
    while (true) {
        Key key = RawMode::readKey();
        if (key == Key::Up)    { cursor = (cursor - 1 + total) % total; draw(); }
        if (key == Key::Down)  { cursor = (cursor + 1) % total; draw(); }
        if (key == Key::Space) { checked[cursor] = !checked[cursor]; draw(); }
        if (key == Key::Enter) {
            std::vector<int> selected;
            for (int i = 0; i < total; i++) {
                if (checked[i]) selected.push_back(i);
            }
            if (selected.empty()) continue; // don't proceed with nothing selected
            ansi::showCursor();
            return selected;
        }
        if (key == Key::Quit) { ansi::showCursor(); return {}; }
    }
}

// ─── Confirmation ────────────────────────────────────────────────────────────

static bool confirmDeletion(
    std::vector<DiskVariant>& disks,
    const std::vector<int>& selected,
    const std::string& methodTitle
) {
    ansi::clearScreen();
    printHeader();

    std::cout << ansi::bold << ansi::red
              << "  ⚠  WARNING: PERMANENT DATA DESTRUCTION  ⚠\n\n"
              << ansi::reset;

    std::cout << ansi::bold << "  Method: " << ansi::cyan << methodTitle << ansi::reset << "\n";
    std::cout << ansi::bold << "  Disks:\n" << ansi::reset;

    for (int idx : selected) {
        auto& d = diskRef(disks[idx]);
        std::cout << ansi::red << "    • " << ansi::reset << ansi::bold << d.getPath() << ansi::reset
                  << " (" << d.getModel() << ", " << formatBytes(d.getSize()) << ")\n";
    }

    std::cout << "\n" << ansi::bold << ansi::yellow
              << "  Type 'yes' to confirm: " << ansi::reset;

    ansi::showCursor();
    std::string input;
    std::getline(std::cin, input);

    return input == "yes";
}

// ─── Screen 3: Deletion Progress ─────────────────────────────────────────────

class ConsoleDelegate : public DiskManagement::DiskDeleteMethod::Delegate {
public:
    void onStageStarted(const DiskManagement::Stage& stage) override {
        std::cout << ansi::bold << ansi::blue
                  << "    [" << (stage.index + 1) << "/" << stage.total << "] "
                  << ansi::reset << stage.name << "...\n" << std::flush;
    }

    void onProgress(const DiskManagement::Stage&, const DiskManagement::Progress& progress) override {
        if (progress.totalBytes() == 0) return;

        double frac = progress.fractionCompleted();
        int width = 40;
        int filled = static_cast<int>(frac * width);

        std::cout << "\r    " << ansi::cyan;
        for (int i = 0; i < width; i++) {
            if (i < filled) std::cout << "█";
            else if (i == filled) std::cout << "▓";
            else std::cout << ansi::dim << "░" << ansi::reset << ansi::cyan;
        }
        std::cout << ansi::reset << " "
                  << std::fixed << std::setprecision(1) << (frac * 100.0) << "%"
                  << std::flush;
    }

    void onStageCompleted(const DiskManagement::Stage&) override {
        std::cout << "\r    " << ansi::cyan;
        for (int i = 0; i < 40; i++) std::cout << "█";
        std::cout << ansi::reset << " 100.0%\n" << std::flush;
    }

    void onCompleted() override {
        std::cout << ansi::green << ansi::bold << "    ✓ Completed\n" << ansi::reset << std::flush;
    }

    void onError(const std::string& message) override {
        std::cout << "\n" << ansi::red << ansi::bold << "    ✗ Error: " << ansi::reset
                  << ansi::red << message << ansi::reset << "\n" << std::flush;
    }
};

static void runDeletion(
    DiskManagement::DiskDeleteMethod* method,
    std::vector<DiskVariant>& disks,
    const std::vector<int>& selected
) {
    ansi::clearScreen();
    printHeader("Sanitization in progress...");

    std::cout << ansi::bold << "  Method: " << ansi::cyan << method->getTitle() << ansi::reset << "\n\n";

    ConsoleDelegate delegate;

    for (size_t i = 0; i < selected.size(); i++) {
        int idx = selected[i];
        auto& d = diskRef(disks[idx]);
        std::string typeCol = diskTypeColor(disks[idx]);

        std::cout << ansi::bold << "  [" << (i + 1) << "/" << selected.size() << "] "
                  << ansi::reset << ansi::bold << d.getPath() << ansi::reset
                  << " (" << d.getModel() << ", " << formatBytes(d.getSize())
                  << " " << typeCol << diskTypeLabel(disks[idx]) << ansi::reset << ")\n";

        std::visit([&](auto& disk) {
            method->deleteDisk(disk, delegate);
        }, disks[idx]);

        std::cout << "\n";
    }

    std::cout << ansi::bold << ansi::green
              << "  ══════════════════════════════════════\n"
              << "  ✓ All operations complete.\n"
              << "  ══════════════════════════════════════\n"
              << ansi::reset;
}

// ─── Main ────────────────────────────────────────────────────────────────────

int main() {
    // Screen 1: Select method
    int methodIndex = selectMethod();
    if (methodIndex < 0) return 0;

    DiskManagement::DiskDeleteMethod* method = DiskManagement::methods[methodIndex];

    // Fetch disks
    ansi::clearScreen();
    printHeader();
    std::cout << ansi::dim << "  Fetching disks..." << ansi::reset << std::flush;
    std::vector<DiskVariant> disks = DiskManagement::fetchDisks();

    // Screen 2: Select disks
    std::vector<int> selected = selectDisks(disks, method->getTitle());
    if (selected.empty()) return 0;

    // Confirm
    if (!confirmDeletion(disks, selected, method->getTitle())) {
        std::cout << ansi::dim << "\n  Aborted.\n" << ansi::reset;
        return 0;
    }

    // Screen 3: Run deletion with progress
    runDeletion(method, disks, selected);

    return 0;
}

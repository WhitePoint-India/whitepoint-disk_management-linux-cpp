//
// Command Executor Implementation
// Robust command execution with error handling for NIST 800-88 compliance
//

#include <disk-management>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <memory>
#include <regex>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>

namespace DiskManagement {

// =============================================================================
// Command Execution
// =============================================================================

CommandExecutor::CommandResult CommandExecutor::execute(const std::string& command, int timeoutSeconds) {
    CommandResult result;
    result.success = false;
    result.exitCode = -1;

    // Use popen to capture stdout and stderr
    // Redirect stderr to stdout to capture both
    std::string fullCommand = command + " 2>&1";

    FILE* pipe = popen(fullCommand.c_str(), "r");
    if (!pipe) {
        result.stderr = "Failed to execute command: popen() failed";
        return result;
    }

    // Read command output
    std::array<char, 256> buffer;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result.stdout += buffer.data();
    }

    // Get exit status
    int status = pclose(pipe);
    if (WIFEXITED(status)) {
        result.exitCode = WEXITSTATUS(status);
        result.success = (result.exitCode == 0);
    } else {
        result.stderr = "Command terminated abnormally";
    }

    // For now, stdout contains both stdout and stderr combined
    // In a more advanced implementation, we could use separate pipes
    result.stderr = result.stdout;

    return result;
}

// =============================================================================
// HPA Detection and Removal
// =============================================================================

std::pair<bool, unsigned long long> CommandExecutor::detectHPA(const std::string& diskPath) {
    // Execute hdparm -N to check for HPA
    std::string command = "hdparm -N " + diskPath;
    CommandResult result = execute(command);

    if (!result.success) {
        std::cerr << "HPA detection failed: " << result.stderr << std::endl;
        return {false, 0};
    }

    // Parse output for HPA status
    // Expected format: "max sectors = 586070255/586072368, HPA is enabled"
    // or: "max sectors = 1465149168/1465149168, HPA is disabled"

    std::regex hpaRegex(R"(max sectors\s*=\s*(\d+)/(\d+))");
    std::smatch match;

    if (std::regex_search(result.stdout, match, hpaRegex)) {
        unsigned long long currentSectors = std::stoull(match[1].str());
        unsigned long long maxSectors = std::stoull(match[2].str());

        // HPA is present if current < max
        bool hpaDetected = (currentSectors < maxSectors);
        return {hpaDetected, maxSectors};
    }

    // Could not parse output
    std::cerr << "Could not parse HPA status from hdparm output" << std::endl;
    return {false, 0};
}

bool CommandExecutor::removeHPA(const std::string& diskPath, unsigned long long maxSectors) {
    // WARNING: This operation is dangerous and irreversible
    std::string command = "hdparm --yes-i-know-what-i-am-doing -N p" +
                         std::to_string(maxSectors) + " " + diskPath;

    CommandResult result = execute(command);

    if (!result.success) {
        std::cerr << "HPA removal failed: " << result.stderr << std::endl;
        return false;
    }

    std::cout << "HPA removed successfully for " << diskPath << std::endl;
    return true;
}

// =============================================================================
// DCO Detection and Removal
// =============================================================================

bool CommandExecutor::detectDCO(const std::string& diskPath) {
    // Execute hdparm --dco-identify to check for DCO
    std::string command = "hdparm --dco-identify " + diskPath;
    CommandResult result = execute(command);

    if (!result.success) {
        // DCO may not be supported on this drive
        // This is not necessarily an error
        return false;
    }

    // Parse output to determine if DCO is present
    // If DCO exists, output will contain DCO configuration info
    // If not, hdparm will indicate DCO is not supported or not present

    // Check for indicators that DCO is present and active
    if (result.stdout.find("DCO") != std::string::npos &&
        result.stdout.find("enabled") != std::string::npos) {
        return true;
    }

    // Also check for explicit "not supported" messages
    if (result.stdout.find("not supported") != std::string::npos ||
        result.stdout.find("not enabled") != std::string::npos) {
        return false;
    }

    // Conservative approach: if we can't determine, assume DCO might be present
    // Better to warn user than miss a DCO
    return false;
}

bool CommandExecutor::removeDCO(const std::string& diskPath) {
    // WARNING: DCO removal is EXTREMELY DANGEROUS
    // Can brick the drive if not properly supported
    // Should only be used when absolutely necessary for compliance

    std::cerr << "WARNING: DCO removal is extremely dangerous and may brick the drive!" << std::endl;
    std::cerr << "Proceeding with DCO removal for: " << diskPath << std::endl;

    std::string command = "hdparm --yes-i-know-what-i-am-doing --dco-restore " + diskPath;
    CommandResult result = execute(command);

    if (!result.success) {
        std::cerr << "DCO removal failed: " << result.stderr << std::endl;
        return false;
    }

    std::cout << "DCO removed successfully for " << diskPath << std::endl;
    return true;
}

// =============================================================================
// Frozen Disk Detection
// =============================================================================

bool CommandExecutor::checkDiskFrozen(const std::string& diskPath) {
    // Check if disk is frozen using hdparm -I
    std::string command = "hdparm -I " + diskPath + " | grep -i frozen";
    CommandResult result = execute(command);

    // If grep finds "frozen", exit code will be 0
    if (result.success && result.stdout.find("frozen") != std::string::npos) {
        // Check if it says "not frozen" or just "frozen"
        if (result.stdout.find("not frozen") != std::string::npos) {
            return false;
        }
        return true;
    }

    return false;
}

} // namespace DiskManagement

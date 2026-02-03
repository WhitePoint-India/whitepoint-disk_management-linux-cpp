//
// Audit Logger Implementation
// NIST 800-88 compliant audit trail with JSON structured logging
//

#include <disk-management>
#include <nlohmann/json.hpp>
#include <disk-management>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <filesystem>

namespace DiskManagement {

// =============================================================================
// Constructor
// =============================================================================

AuditLogger::AuditLogger(const Disk& disk, const std::string& method) {
    // Generate log ID
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    std::ostringstream logIdStream;
    logIdStream << "LOG-" << std::put_time(std::localtime(&timestamp), "%Y%m%d-%H%M%S")
                << "-" << disk.serial;
    logId = logIdStream.str();

    // Initialize JSON log with basic information
    logData["timestamp"] = getCurrentTimestamp();
    logData["operator"] = getOperator();
    logData["hostname"] = getHostname();

    // Hardware information
    logData["hardware"]["manufacturer"] = "Unknown"; // Not available in current Disk class
    logData["hardware"]["model"] = disk.model;
    logData["hardware"]["serial"] = disk.serial;
    logData["hardware"]["path"] = disk.path;
    logData["hardware"]["description"] = disk.description;
    logData["hardware"]["capacity_bytes"] = disk.size;
    logData["hardware"]["sector_size"] = disk.sectorSize;

    // Determine disk type from description or path
    std::string diskType = "Unknown";
    if (disk.path.find("nvme") != std::string::npos) {
        diskType = "NVMe";
    } else if (disk.path.find("sd") != std::string::npos) {
        diskType = "ATA/SATA";
    }
    logData["hardware"]["type"] = diskType;

    // Sanitization information
    logData["sanitization"]["method"] = method;
    logData["sanitization"]["started_at"] = getCurrentTimestamp();

    // Initialize HPA/DCO status as not checked
    logData["sanitization"]["hpa_detected"] = false;
    logData["sanitization"]["hpa_removed"] = false;
    logData["sanitization"]["dco_detected"] = false;
    logData["sanitization"]["dco_removed"] = false;

    // Verification placeholder
    logData["verification"]["status"] = "TODO_NOT_IMPLEMENTED";
    logData["verification"]["notes"] = "Verification to be implemented in future phase";

    logData["log_id"] = logId;
}

// =============================================================================
// Log Methods
// =============================================================================

void AuditLogger::logHPAStatus(bool detected, bool removed, unsigned long long hiddenSectors) {
    logData["sanitization"]["hpa_detected"] = detected;
    logData["sanitization"]["hpa_removed"] = removed;
    if (detected) {
        logData["sanitization"]["hpa_hidden_sectors"] = hiddenSectors;
    }
}

void AuditLogger::logDCOStatus(bool detected, bool removed) {
    logData["sanitization"]["dco_detected"] = detected;
    logData["sanitization"]["dco_removed"] = removed;
}

void AuditLogger::logCommandExecution(const std::string& command,
                                     const CommandExecutor::CommandResult& result) {
    // Store last executed command and its result
    logData["sanitization"]["command"] = command;
    logData["sanitization"]["command_exit_code"] = result.exitCode;
    logData["sanitization"]["command_success"] = result.success;

    // Store command output (truncated if too long)
    std::string output = result.stdout;
    if (output.length() > 1000) {
        output = output.substr(0, 1000) + "... [truncated]";
    }
    logData["sanitization"]["command_output"] = output;
}

void AuditLogger::logVerification(const VerificationResult& result) {
    // Log verification results
    logData["verification"]["status"] = result.passed ? "PASSED" : "FAILED";

    std::string levelStr;
    switch (result.level) {
        case VerificationLevel::NONE:
            levelStr = "NONE";
            break;
        case VerificationLevel::SAMPLING:
            levelStr = "SAMPLING";
            break;
        case VerificationLevel::FULL:
            levelStr = "FULL";
            break;
        case VerificationLevel::DEVICE_LOG:
            levelStr = "DEVICE_LOG";
            break;
    }

    logData["verification"]["level"] = levelStr;
    logData["verification"]["message"] = result.message;
    logData["verification"]["sectors_verified"] = result.sectorsVerified;
    logData["verification"]["sectors_total"] = result.sectorsTotal;

    if (result.sectorsTotal > 0) {
        double percentVerified = (static_cast<double>(result.sectorsVerified) / result.sectorsTotal) * 100.0;
        logData["verification"]["percent_verified"] = percentVerified;
    }
}

void AuditLogger::logCompletion(const std::string& status) {
    logData["sanitization"]["completed_at"] = getCurrentTimestamp();
    logData["sanitization"]["status"] = status;

    // Calculate duration
    std::string startTime = logData["sanitization"]["started_at"];
    std::string endTime = logData["sanitization"]["completed_at"];
    // For simplicity, duration calculation is omitted
    // In production, parse timestamps and calculate difference
    logData["sanitization"]["duration_seconds"] = 0; // TODO: Calculate actual duration
}

std::string AuditLogger::finalize() {
    // Ensure log directory exists
    const std::string logDir = "/var/log/nist-sanitization";

    // Create directory if it doesn't exist
    try {
        std::filesystem::create_directories(logDir);
        // Set permissions to 700 (owner read/write/execute only)
        chmod(logDir.c_str(), S_IRWXU);
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not create log directory: " << e.what() << std::endl;
        std::cerr << "Attempting to write log to current directory instead" << std::endl;
    }

    // Generate log filename
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    std::ostringstream filenameStream;
    filenameStream << logDir << "/"
                   << std::put_time(std::localtime(&timestamp), "%Y-%m-%d_%H%M%S")
                   << "_" << logData["hardware"]["serial"].get<std::string>()
                   << ".json";
    std::string filename = filenameStream.str();

    // Write log file
    try {
        std::ofstream logFile(filename);
        if (!logFile.is_open()) {
            // Fallback to current directory
            filename = logId + ".json";
            logFile.open(filename);
            if (!logFile.is_open()) {
                throw std::runtime_error("Could not open log file for writing");
            }
        }

        // Write JSON with pretty formatting (4 spaces)
        logFile << std::setw(4) << logData << std::endl;
        logFile.close();

        std::cout << "Audit log written to: " << filename << std::endl;
        return logId;
    } catch (const std::exception& e) {
        std::cerr << "Error writing audit log: " << e.what() << std::endl;
        throw;
    }
}

// =============================================================================
// Helper Methods
// =============================================================================

std::string AuditLogger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&timestamp), "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::string AuditLogger::getHostname() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return std::string(hostname);
    }
    return "unknown";
}

std::string AuditLogger::getOperator() {
    const char* username = getenv("USER");
    if (username != nullptr) {
        return std::string(username);
    }
    const char* sudo_user = getenv("SUDO_USER");
    if (sudo_user != nullptr) {
        return std::string(sudo_user) + " (via sudo)";
    }
    return "root";
}

} // namespace DiskManagement

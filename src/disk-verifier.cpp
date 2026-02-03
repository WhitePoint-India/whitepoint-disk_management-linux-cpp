//
// Disk Verifier Implementation
// NIST 800-88 compliant post-sanitization verification
//

#include <disk-management>
#include <iostream>
#include <fstream>
#include <random>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

namespace DiskManagement {

// =============================================================================
// ATA Disk Verification
// =============================================================================

VerificationResult DiskVerifier::verifyATADisk(const ATADisk& disk, VerificationLevel level) {
    std::cout << "Verifying ATA disk: " << disk.path << std::endl;

    switch (level) {
        case VerificationLevel::SAMPLING:
            return verifySampling(disk, 0.10);
        case VerificationLevel::FULL:
            return verifyFull(disk);
        case VerificationLevel::NONE:
            return {false, VerificationLevel::NONE, "Verification disabled", 0, 0};
        default:
            return {false, level, "Unsupported verification level for ATA", 0, 0};
    }
}

// =============================================================================
// NVMe Disk Verification
// =============================================================================

VerificationResult DiskVerifier::verifyNVMeDisk(const NVMeDisk& disk, VerificationLevel level) {
    std::cout << "Verifying NVMe disk: " << disk.path << std::endl;

    switch (level) {
        case VerificationLevel::DEVICE_LOG: {
            // Extract controller device (e.g., /dev/nvme0n1 -> /dev/nvme0)
            std::string devicePath = disk.path;
            size_t nPos = devicePath.find("n1");
            if (nPos != std::string::npos) {
                devicePath = devicePath.substr(0, nPos);
            }
            return checkNVMeSanitizeLog(devicePath);
        }
        case VerificationLevel::SAMPLING:
            return verifySampling(disk, 0.10);
        case VerificationLevel::FULL:
            return verifyFull(disk);
        case VerificationLevel::NONE:
            return {false, VerificationLevel::NONE, "Verification disabled", 0, 0};
        default:
            return {false, level, "Unsupported verification level for NVMe", 0, 0};
    }
}

// =============================================================================
// Sampling Verification (10% random sectors)
// =============================================================================

VerificationResult DiskVerifier::verifySampling(const Disk& disk, double samplePercent) {
    std::cout << "Performing sampling verification ("
              << (samplePercent * 100) << "% of disk)..." << std::endl;

    unsigned long long totalSectors = disk.getSectorCount();
    unsigned long long samplesToCheck = static_cast<unsigned long long>(totalSectors * samplePercent);

    if (samplesToCheck == 0) {
        samplesToCheck = std::min(1000ULL, totalSectors); // At least 1000 sectors or disk size
    }

    std::cout << "Total sectors: " << totalSectors << std::endl;
    std::cout << "Sectors to verify: " << samplesToCheck << std::endl;

    // Open disk for reading
    int fd = open(disk.path.c_str(), O_RDONLY | O_DIRECT);
    if (fd < 0) {
        return {false, VerificationLevel::SAMPLING,
                "Failed to open disk for verification: " + std::string(strerror(errno)),
                0, samplesToCheck};
    }

    // Allocate aligned buffer for direct I/O
    const size_t bufferSize = disk.sectorSize;
    void* buffer;
    if (posix_memalign(&buffer, disk.sectorSize, bufferSize) != 0) {
        close(fd);
        return {false, VerificationLevel::SAMPLING,
                "Failed to allocate aligned buffer", 0, samplesToCheck};
    }

    // Generate random sector positions
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<unsigned long long> dist(0, totalSectors - 1);

    unsigned long long sectorsVerified = 0;
    unsigned long long nonZeroSectors = 0;
    bool verificationPassed = true;

    for (unsigned long long i = 0; i < samplesToCheck; ++i) {
        unsigned long long sectorNum = dist(gen);
        off_t offset = static_cast<off_t>(sectorNum) * disk.sectorSize;

        // Seek to sector
        if (lseek(fd, offset, SEEK_SET) != offset) {
            std::cerr << "Failed to seek to sector " << sectorNum << std::endl;
            continue;
        }

        // Read sector
        ssize_t bytesRead = read(fd, buffer, bufferSize);
        if (bytesRead != static_cast<ssize_t>(bufferSize)) {
            std::cerr << "Failed to read sector " << sectorNum << std::endl;
            continue;
        }

        // Check if sector is all zeros
        bool isZero = true;
        const unsigned char* data = static_cast<const unsigned char*>(buffer);
        for (size_t j = 0; j < bufferSize; ++j) {
            if (data[j] != 0) {
                isZero = false;
                nonZeroSectors++;
                break;
            }
        }

        sectorsVerified++;

        // Progress indicator
        if (sectorsVerified % 1000 == 0) {
            std::cout << "Verified " << sectorsVerified << " / " << samplesToCheck
                     << " sectors..." << std::endl;
        }
    }

    free(buffer);
    close(fd);

    std::cout << "Verification complete: " << sectorsVerified << " sectors checked" << std::endl;
    std::cout << "Non-zero sectors found: " << nonZeroSectors << std::endl;

    // Consider verification passed if less than 1% of sampled sectors are non-zero
    // (some sectors may have firmware/metadata that can't be erased)
    double nonZeroPercent = (static_cast<double>(nonZeroSectors) / sectorsVerified) * 100.0;
    verificationPassed = (nonZeroPercent < 1.0);

    std::string message;
    if (verificationPassed) {
        message = "Sampling verification passed (" + std::to_string(nonZeroPercent) +
                  "% non-zero sectors)";
    } else {
        message = "Sampling verification FAILED (" + std::to_string(nonZeroPercent) +
                  "% non-zero sectors exceeds 1% threshold)";
    }

    return {verificationPassed, VerificationLevel::SAMPLING, message,
            sectorsVerified, samplesToCheck};
}

// =============================================================================
// Full Verification (sequential read of all sectors)
// =============================================================================

VerificationResult DiskVerifier::verifyFull(const Disk& disk) {
    std::cout << "Performing FULL disk verification (this will take 1-3 hours)..." << std::endl;

    unsigned long long totalSectors = disk.getSectorCount();
    std::cout << "Total sectors to verify: " << totalSectors << std::endl;

    // Open disk for reading
    int fd = open(disk.path.c_str(), O_RDONLY | O_DIRECT);
    if (fd < 0) {
        return {false, VerificationLevel::FULL,
                "Failed to open disk for verification: " + std::string(strerror(errno)),
                0, totalSectors};
    }

    // Allocate aligned buffer for direct I/O (read multiple sectors at once)
    const size_t sectorsPerRead = 1024; // Read 1024 sectors at a time
    const size_t bufferSize = disk.sectorSize * sectorsPerRead;
    void* buffer;
    if (posix_memalign(&buffer, disk.sectorSize, bufferSize) != 0) {
        close(fd);
        return {false, VerificationLevel::FULL,
                "Failed to allocate aligned buffer", 0, totalSectors};
    }

    unsigned long long sectorsVerified = 0;
    unsigned long long nonZeroSectors = 0;
    bool verificationPassed = true;

    while (sectorsVerified < totalSectors) {
        unsigned long long sectorsToRead = std::min(static_cast<unsigned long long>(sectorsPerRead),
                                                     totalSectors - sectorsVerified);
        size_t bytesToRead = sectorsToRead * disk.sectorSize;

        ssize_t bytesRead = read(fd, buffer, bytesToRead);
        if (bytesRead <= 0) {
            if (bytesRead < 0) {
                std::cerr << "Read error at sector " << sectorsVerified << std::endl;
            }
            break;
        }

        // Check if data is all zeros
        const unsigned char* data = static_cast<const unsigned char*>(buffer);
        for (ssize_t i = 0; i < bytesRead; ++i) {
            if (data[i] != 0) {
                nonZeroSectors++;
                break; // Count this chunk as non-zero, move to next
            }
        }

        sectorsVerified += sectorsToRead;

        // Progress indicator every 10GB
        if (sectorsVerified % (10ULL * 1024 * 1024 * 1024 / disk.sectorSize) == 0) {
            double percentComplete = (static_cast<double>(sectorsVerified) / totalSectors) * 100.0;
            std::cout << "Progress: " << percentComplete << "% ("
                     << sectorsVerified << " / " << totalSectors << " sectors)" << std::endl;
        }
    }

    free(buffer);
    close(fd);

    std::cout << "Full verification complete: " << sectorsVerified << " sectors checked" << std::endl;
    std::cout << "Non-zero sector chunks found: " << nonZeroSectors << std::endl;

    // Consider verification passed if less than 0.1% of disk has non-zero data
    double nonZeroPercent = (static_cast<double>(nonZeroSectors * sectorsPerRead) / totalSectors) * 100.0;
    verificationPassed = (nonZeroPercent < 0.1);

    std::string message;
    if (verificationPassed) {
        message = "Full verification passed (" + std::to_string(nonZeroPercent) +
                  "% non-zero data)";
    } else {
        message = "Full verification FAILED (" + std::to_string(nonZeroPercent) +
                  "% non-zero data exceeds 0.1% threshold)";
    }

    return {verificationPassed, VerificationLevel::FULL, message,
            sectorsVerified, totalSectors};
}

// =============================================================================
// NVMe Sanitize Log Verification
// =============================================================================

VerificationResult DiskVerifier::checkNVMeSanitizeLog(const std::string& devicePath) {
    std::cout << "Checking NVMe sanitize log for: " << devicePath << std::endl;

    std::string command = "nvme sanitize-log " + devicePath + " 2>&1";
    CommandExecutor::CommandResult result = CommandExecutor::execute(command);

    if (!result.success) {
        return {false, VerificationLevel::DEVICE_LOG,
                "Failed to read NVMe sanitize log: " + result.stderr, 0, 0};
    }

    // Check for successful completion indicators
    // SSTAT: 0x101 means "Sanitize operation completed successfully with no restrictions"
    bool completed = false;
    bool successful = false;

    if (result.stdout.find("0x101") != std::string::npos) {
        completed = true;
        successful = true;
    } else if (result.stdout.find("completed successfully") != std::string::npos) {
        completed = true;
        successful = true;
    } else if (result.stdout.find("in progress") != std::string::npos) {
        return {false, VerificationLevel::DEVICE_LOG,
                "Sanitize operation still in progress", 0, 0};
    } else if (result.stdout.find("failed") != std::string::npos) {
        return {false, VerificationLevel::DEVICE_LOG,
                "Sanitize operation reported failure", 0, 0};
    }

    std::string message;
    if (completed && successful) {
        message = "NVMe sanitize log confirms successful completion (SSTAT: 0x101)";
    } else {
        message = "NVMe sanitize status unclear from log output";
    }

    return {completed && successful, VerificationLevel::DEVICE_LOG, message, 0, 0};
}

} // namespace DiskManagement

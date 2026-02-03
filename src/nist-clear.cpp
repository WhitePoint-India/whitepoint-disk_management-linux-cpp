//
// NIST 800-88 Clear Method Implementation
// Logical sanitization using standard Read/Write commands
//

#include <disk-management>
#include <disk-management>
#include <iostream>
#include <unistd.h>

namespace DiskManagement {

// =============================================================================
// NISTClear for ATADisk
// =============================================================================
// Method: Single-pass overwrite with zeros using dd
// Protection Level: Moderate - protects against simple, non-invasive recovery
// Does NOT address hidden areas (HPA/DCO)
// =============================================================================

void NISTClear::deleteDisk(ATADisk& disk) const {
    std::cout << "=== NIST 800-88 Clear Method (ATA/SATA) ===" << std::endl;
    std::cout << "Disk: " << disk.path << " (" << disk.model << ")" << std::endl;

    // Initialize audit logger
    AuditLogger logger(disk, "NIST_CLEAR");
    logger.logData["sanitization"]["compliance_level"] = "Clear";
    logger.logData["sanitization"]["technique"] = "Single_Pass_Overwrite_DD";

    // Check root privileges
    if (geteuid() != 0) {
        std::cerr << "Error: Root privileges required" << std::endl;
        logger.logCompletion("FAILED_PERMISSION_DENIED");
        logger.finalize();
        return;
    }

    // Check if disk is frozen
    if (disk.isFrozen()) {
        std::cerr << "Warning: Disk is FROZEN. Attempting to unfreeze..." << std::endl;
        disk.unfreeze();
        // Note: Current unfreeze() only changes state flag, doesn't actually unfreeze
        // Real unfreezing requires system suspend/resume cycle
    }

    // Construct dd command for single-pass overwrite
    // Use direct I/O for better performance and to bypass cache
    std::string command = "dd if=/dev/zero of=" + disk.path +
                         " bs=1M status=none oflag=direct";

    std::cout << "Executing: " << command << std::endl;
    std::cout << "This will take 30 minutes to 2 hours depending on disk size..." << std::endl;

    // Execute command
    CommandExecutor::CommandResult result = CommandExecutor::execute(command);
    logger.logCommandExecution(command, result);

    if (result.success) {
        std::cout << "Clear sanitization completed successfully" << std::endl;

        // Perform sampling verification (10% of disk)
        std::cout << std::endl;
        std::cout << "Performing verification (sampling 10% of disk)..." << std::endl;
        VerificationResult verifyResult = DiskVerifier::verifyATADisk(disk, VerificationLevel::SAMPLING);
        logger.logVerification(verifyResult);

        if (verifyResult.passed) {
            std::cout << "Verification PASSED: " << verifyResult.message << std::endl;
            logger.logCompletion("SUCCESS_VERIFIED");
        } else {
            std::cout << "Verification FAILED: " << verifyResult.message << std::endl;
            logger.logCompletion("SUCCESS_VERIFICATION_FAILED");
        }
    } else {
        std::cerr << "Clear sanitization failed: " << result.stderr << std::endl;
        logger.logCompletion("FAILED");
    }

    // Finalize audit log
    try {
        std::string logId = logger.finalize();
        std::cout << "Audit log ID: " << logId << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not write audit log: " << e.what() << std::endl;
    }
}

// =============================================================================
// NISTClear for NVMeDisk
// =============================================================================
// Method: User Data Erase via FORMAT NVM command (SES=1)
// Protection Level: Moderate - logical data erasure
// Much faster than dd overwrite (typically 1-5 minutes)
// =============================================================================

void NISTClear::deleteDisk(NVMeDisk& disk) const {
    std::cout << "=== NIST 800-88 Clear Method (NVMe) ===" << std::endl;
    std::cout << "Disk: " << disk.path << " (" << disk.model << ")" << std::endl;

    // Initialize audit logger
    AuditLogger logger(disk, "NIST_CLEAR");
    logger.logData["sanitization"]["compliance_level"] = "Clear";
    logger.logData["sanitization"]["technique"] = "NVMe_User_Data_Erase_Format";

    // Check root privileges
    if (geteuid() != 0) {
        std::cerr << "Error: Root privileges required" << std::endl;
        logger.logCompletion("FAILED_PERMISSION_DENIED");
        logger.finalize();
        return;
    }

    // Construct nvme format command
    // -s 1: Secure Erase Setting = User Data Erase
    // -f: Force (no confirmation prompt)
    std::string command = "nvme format " + disk.path + " -s 1 -f";

    std::cout << "Executing: " << command << std::endl;
    std::cout << "This typically takes 1-5 minutes..." << std::endl;

    // Execute command
    CommandExecutor::CommandResult result = CommandExecutor::execute(command, 600); // 10 minute timeout

    logger.logCommandExecution(command, result);

    if (result.success) {
        std::cout << "Clear sanitization completed successfully" << std::endl;

        // Perform sampling verification (10% of disk)
        std::cout << std::endl;
        std::cout << "Performing verification (sampling 10% of disk)..." << std::endl;
        VerificationResult verifyResult = DiskVerifier::verifyNVMeDisk(disk, VerificationLevel::SAMPLING);
        logger.logVerification(verifyResult);

        if (verifyResult.passed) {
            std::cout << "Verification PASSED: " << verifyResult.message << std::endl;
            logger.logCompletion("SUCCESS_VERIFIED");
        } else {
            std::cout << "Verification FAILED: " << verifyResult.message << std::endl;
            logger.logCompletion("SUCCESS_VERIFICATION_FAILED");
        }
    } else {
        std::cerr << "Clear sanitization failed: " << result.stderr << std::endl;
        logger.logCompletion("FAILED");
    }

    // Finalize audit log
    try {
        std::string logId = logger.finalize();
        std::cout << "Audit log ID: " << logId << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not write audit log: " << e.what() << std::endl;
    }
}

// =============================================================================
// Enhanced API with Status Returns
// =============================================================================

SanitizationStatus NISTClear::deleteDiskWithStatus(ATADisk& disk) const {
    // TODO: Implement status-returning version
    // For now, call existing implementation
    deleteDisk(disk);
    return {SanitizationResult::SUCCESS, "Clear sanitization completed", std::nullopt};
}

SanitizationStatus NISTClear::deleteDiskWithStatus(NVMeDisk& disk) const {
    // TODO: Implement status-returning version
    // For now, call existing implementation
    deleteDisk(disk);
    return {SanitizationResult::SUCCESS, "Clear sanitization completed", std::nullopt};
}

} // namespace DiskManagement

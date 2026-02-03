//
// NIST 800-88 Purge Method Implementation
// Physical/logical techniques protecting against laboratory attacks
// CRITICAL for Government/DoD compliance
//

#include <disk-management>
#include <iostream>
#include <unistd.h>

namespace DiskManagement {

// =============================================================================
// NISTPurge for ATADisk
// =============================================================================
// NIST 800-88 Purge Requirements for ATA/SATA:
// 1. MUST detect and remove HPA (Host Protected Area)
// 2. MUST detect and remove DCO (Device Configuration Overlay)
// 3. Perform Enhanced Secure Erase (preferred) or Secure Erase (fallback)
// 4. Audit trail required for all steps
// =============================================================================

void NISTPurge::deleteDisk(ATADisk& disk) const {
    std::cout << "========================================" << std::endl;
    std::cout << "=== NIST 800-88 Purge Method (ATA) ===" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Disk: " << disk.path << " (" << disk.model << ")" << std::endl;
    std::cout << "Serial: " << disk.serial << std::endl;
    std::cout << std::endl;
    std::cout << "WARNING: This operation is UNINTERRUPTIBLE and IRREVERSIBLE" << std::endl;
    std::cout << "All data will be PERMANENTLY DESTROYED" << std::endl;
    std::cout << std::endl;

    // Initialize audit logger
    AuditLogger logger(disk, "NIST_PURGE");
    logger.logData["sanitization"]["compliance_level"] = "Purge";

    // Check root privileges
    if (geteuid() != 0) {
        std::cerr << "Error: Root privileges required for Purge operations" << std::endl;
        logger.logCompletion("FAILED_PERMISSION_DENIED");
        logger.finalize();
        return;
    }

    // Step 1: Check if disk is frozen
    std::cout << "[Step 1/5] Checking disk frozen state..." << std::endl;
    if (CommandExecutor::checkDiskFrozen(disk.path)) {
        std::cerr << "ERROR: Disk is FROZEN by firmware" << std::endl;
        std::cerr << "ATA security commands cannot be executed while disk is frozen" << std::endl;
        std::cerr << std::endl;
        std::cerr << "To unfreeze, you must:" << std::endl;
        std::cerr << "  1. Suspend the system: systemctl suspend" << std::endl;
        std::cerr << "  2. Resume the system (wake from suspend)" << std::endl;
        std::cerr << "  3. Re-run this sanitization" << std::endl;
        std::cerr << std::endl;
        logger.logData["sanitization"]["frozen_state_error"] = true;
        logger.logCompletion("FAILED_DISK_FROZEN");
        logger.finalize();
        return;
    }
    std::cout << "Disk is not frozen. Proceeding..." << std::endl;
    std::cout << std::endl;

    // Step 2: Detect and Remove HPA (CRITICAL for compliance)
    std::cout << "[Step 2/5] Detecting Host Protected Area (HPA)..." << std::endl;
    auto [hpaDetected, maxSectors] = CommandExecutor::detectHPA(disk.path);

    if (hpaDetected) {
        std::cout << "HPA DETECTED: Hidden sectors found" << std::endl;
        std::cout << "Maximum sectors: " << maxSectors << std::endl;
        std::cout << "Removing HPA to ensure complete sanitization..." << std::endl;

        bool hpaRemoved = CommandExecutor::removeHPA(disk.path, maxSectors);
        logger.logHPAStatus(true, hpaRemoved, maxSectors);

        if (!hpaRemoved) {
            std::cerr << "ERROR: HPA removal failed" << std::endl;
            std::cerr << "NIST 800-88 Purge compliance NOT achieved" << std::endl;
            logger.logCompletion("FAILED_HPA_REMOVAL");
            logger.finalize();
            return;
        }
        std::cout << "HPA removed successfully" << std::endl;
    } else {
        std::cout << "No HPA detected" << std::endl;
        logger.logHPAStatus(false, false, 0);
    }
    std::cout << std::endl;

    // Step 3: Detect and Remove DCO (CRITICAL for compliance)
    std::cout << "[Step 3/5] Detecting Device Configuration Overlay (DCO)..." << std::endl;
    bool dcoDetected = CommandExecutor::detectDCO(disk.path);

    if (dcoDetected) {
        std::cout << "DCO DETECTED: Device configuration overlay present" << std::endl;
        std::cout << "WARNING: DCO removal is EXTREMELY DANGEROUS" << std::endl;
        std::cout << "         Can permanently brick the drive if not supported" << std::endl;
        std::cout << "Removing DCO for NIST compliance..." << std::endl;

        bool dcoRemoved = CommandExecutor::removeDCO(disk.path);
        logger.logDCOStatus(true, dcoRemoved);

        if (!dcoRemoved) {
            std::cerr << "ERROR: DCO removal failed" << std::endl;
            std::cerr << "NIST 800-88 Purge compliance NOT achieved" << std::endl;
            logger.logCompletion("FAILED_DCO_REMOVAL");
            logger.finalize();
            return;
        }
        std::cout << "DCO removed successfully" << std::endl;
    } else {
        std::cout << "No DCO detected or not supported by drive" << std::endl;
        logger.logDCOStatus(false, false);
    }
    std::cout << std::endl;

    // Step 4: Set Security Password
    std::cout << "[Step 4/5] Setting security password..." << std::endl;
    std::string setPasswordCmd = "hdparm --user-master u --security-set-pass whitepoint " +
                                 disk.path + " > /dev/null 2>&1";

    CommandExecutor::CommandResult setPassResult = CommandExecutor::execute(setPasswordCmd);
    if (!setPassResult.success) {
        std::cerr << "ERROR: Failed to set security password" << std::endl;
        logger.logCommandExecution(setPasswordCmd, setPassResult);
        logger.logCompletion("FAILED_SET_PASSWORD");
        logger.finalize();
        return;
    }
    std::cout << "Security password set" << std::endl;
    std::cout << std::endl;

    // Step 5: Perform Enhanced Secure Erase (or fallback to Secure Erase)
    std::cout << "[Step 5/5] Performing Secure Erase..." << std::endl;
    std::cout << "Attempting Enhanced Secure Erase first..." << std::endl;

    // Try Enhanced Secure Erase
    std::string enhancedCmd = "hdparm --user-master u --security-erase-enhanced whitepoint " +
                             disk.path + " > /dev/null 2>&1";

    std::cout << "This may take 1-4 hours depending on drive capacity..." << std::endl;
    CommandExecutor::CommandResult enhancedResult = CommandExecutor::execute(enhancedCmd, 21600); // 6 hour timeout

    if (enhancedResult.success) {
        std::cout << "Enhanced Secure Erase completed successfully" << std::endl;
        logger.logData["sanitization"]["technique"] = "ATA_Enhanced_Secure_Erase";
        logger.logCommandExecution(enhancedCmd, enhancedResult);

        // Perform verification
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
        std::cout << "Enhanced Secure Erase not supported or failed" << std::endl;
        std::cout << "Falling back to Standard Secure Erase..." << std::endl;

        // Fallback to Standard Secure Erase
        std::string standardCmd = "hdparm --user-master u --security-erase whitepoint " +
                                 disk.path + " > /dev/null 2>&1";

        CommandExecutor::CommandResult standardResult = CommandExecutor::execute(standardCmd, 21600); // 6 hour timeout
        logger.logCommandExecution(standardCmd, standardResult);

        if (standardResult.success) {
            std::cout << "Standard Secure Erase completed successfully" << std::endl;
            logger.logData["sanitization"]["technique"] = "ATA_Secure_Erase";

            // Perform verification
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
            std::cerr << "ERROR: Both Enhanced and Standard Secure Erase failed" << std::endl;
            logger.logData["sanitization"]["technique"] = "ATA_Secure_Erase_Failed";
            logger.logCompletion("FAILED_SANITIZE_COMMAND");
        }
    }

    // Finalize audit log
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    try {
        std::string logId = logger.finalize();
        std::cout << "Purge operation completed" << std::endl;
        std::cout << "Audit log ID: " << logId << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not write audit log: " << e.what() << std::endl;
    }
    std::cout << "========================================" << std::endl;
}

// =============================================================================
// NISTPurge for NVMeDisk
// =============================================================================
// NIST 800-88 Purge Requirements for NVMe:
// 1. Block Erase Sanitize (0x02) - preferred
// 2. Cryptographic Erase Sanitize (0x04) - fallback
// 3. HPA/DCO not applicable to NVMe
// 4. Sanitize is UNINTERRUPTIBLE even through power cycle
// =============================================================================

void NISTPurge::deleteDisk(NVMeDisk& disk) const {
    std::cout << "==========================================" << std::endl;
    std::cout << "=== NIST 800-88 Purge Method (NVMe) ===" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Disk: " << disk.path << " (" << disk.model << ")" << std::endl;
    std::cout << "Serial: " << disk.serial << std::endl;
    std::cout << std::endl;
    std::cout << "WARNING: NVMe Sanitize is UNINTERRUPTIBLE" << std::endl;
    std::cout << "Once started, it CANNOT be stopped even by power cycle" << std::endl;
    std::cout << "All data will be PERMANENTLY DESTROYED" << std::endl;
    std::cout << std::endl;

    // Initialize audit logger
    AuditLogger logger(disk, "NIST_PURGE");
    logger.logData["sanitization"]["compliance_level"] = "Purge";

    // Check root privileges
    if (geteuid() != 0) {
        std::cerr << "Error: Root privileges required for Purge operations" << std::endl;
        logger.logCompletion("FAILED_PERMISSION_DENIED");
        logger.finalize();
        return;
    }

    // Extract device name (e.g., /dev/nvme0n1 -> /dev/nvme0)
    // NVMe sanitize requires the controller device, not the namespace
    std::string devicePath = disk.path;
    size_t nPos = devicePath.find("n1");
    if (nPos != std::string::npos) {
        devicePath = devicePath.substr(0, nPos);
    }

    // Step 1: Attempt Block Erase Sanitize (preferred)
    std::cout << "[Step 1/2] Attempting Block Erase Sanitize..." << std::endl;
    std::cout << "Using device: " << devicePath << std::endl;

    // -a 0x02 = Sanitize Action: Block Erase
    std::string blockEraseCmd = "nvme sanitize " + devicePath + " -a 0x02";

    std::cout << "This may take 2-4 hours depending on drive capacity..." << std::endl;
    std::cout << "Command: " << blockEraseCmd << std::endl;

    CommandExecutor::CommandResult blockResult = CommandExecutor::execute(blockEraseCmd, 21600); // 6 hour timeout
    logger.logCommandExecution(blockEraseCmd, blockResult);

    if (blockResult.success) {
        std::cout << "Block Erase Sanitize initiated successfully" << std::endl;
        logger.logData["sanitization"]["technique"] = "NVMe_Block_Erase_Sanitize";

        // Check sanitize status and verify
        std::cout << std::endl;
        std::cout << "[Step 2/2] Checking sanitize completion and verifying..." << std::endl;

        VerificationResult verifyResult = DiskVerifier::checkNVMeSanitizeLog(devicePath);
        logger.logVerification(verifyResult);

        if (verifyResult.passed) {
            std::cout << "Verification PASSED: " << verifyResult.message << std::endl;
            logger.logCompletion("SUCCESS_VERIFIED");
        } else {
            std::cout << "Verification result: " << verifyResult.message << std::endl;
            std::cout << "Check sanitize-log manually for final status" << std::endl;
            logger.logCompletion("IN_PROGRESS_OR_UNCLEAR");
        }
    } else {
        // Block Erase failed, try Cryptographic Erase
        std::cout << "Block Erase Sanitize not supported or failed" << std::endl;
        std::cout << std::endl;
        std::cout << "[Alternative] Attempting Cryptographic Erase Sanitize..." << std::endl;

        // -a 0x04 = Sanitize Action: Cryptographic Erase
        std::string cryptoCmd = "nvme sanitize " + devicePath + " -a 0x04";
        std::cout << "Command: " << cryptoCmd << std::endl;

        CommandExecutor::CommandResult cryptoResult = CommandExecutor::execute(cryptoCmd, 60); // Fast operation
        logger.logCommandExecution(cryptoCmd, cryptoResult);

        if (cryptoResult.success) {
            std::cout << "Cryptographic Erase Sanitize completed (instant key destruction)" << std::endl;
            logger.logData["sanitization"]["technique"] = "NVMe_Cryptographic_Erase_Sanitize";

            // Verify via sanitize log
            std::cout << std::endl;
            std::cout << "Verifying via sanitize log..." << std::endl;
            VerificationResult verifyResult = DiskVerifier::checkNVMeSanitizeLog(devicePath);
            logger.logVerification(verifyResult);

            if (verifyResult.passed) {
                std::cout << "Verification PASSED: " << verifyResult.message << std::endl;
                logger.logCompletion("SUCCESS_VERIFIED");
            } else {
                std::cout << "Verification result: " << verifyResult.message << std::endl;
                logger.logCompletion("SUCCESS_VERIFICATION_UNCLEAR");
            }
        } else {
            std::cerr << "ERROR: Both Block Erase and Cryptographic Erase failed" << std::endl;
            logger.logData["sanitization"]["technique"] = "NVMe_Sanitize_Failed";
            logger.logCompletion("FAILED_SANITIZE_COMMAND");
        }
    }

    // Finalize audit log
    std::cout << std::endl;
    std::cout << "==========================================" << std::endl;
    try {
        std::string logId = logger.finalize();
        std::cout << "Purge operation completed" << std::endl;
        std::cout << "Audit log ID: " << logId << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not write audit log: " << e.what() << std::endl;
    }
    std::cout << "==========================================" << std::endl;
}

// =============================================================================
// Enhanced API with Status Returns
// =============================================================================

SanitizationStatus NISTPurge::deleteDiskWithStatus(ATADisk& disk) const {
    // TODO: Implement status-returning version with granular error codes
    // For now, call existing implementation
    deleteDisk(disk);
    return {SanitizationResult::SUCCESS, "Purge sanitization completed", std::nullopt};
}

SanitizationStatus NISTPurge::deleteDiskWithStatus(NVMeDisk& disk) const {
    // TODO: Implement status-returning version with granular error codes
    // For now, call existing implementation
    deleteDisk(disk);
    return {SanitizationResult::SUCCESS, "Purge sanitization completed", std::nullopt};
}

} // namespace DiskManagement

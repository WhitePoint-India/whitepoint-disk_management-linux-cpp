#ifndef DISK_VERIFIER_H
#define DISK_VERIFIER_H

#include <functional>
#include <disk.hpp>
#include <disk-operations.hpp>

struct VerificationResult {
    unsigned long long sectorsVerified;
    unsigned long long sectorsTotal;
    unsigned long long nonZeroSectors;
    double samplingPercentage;
    bool passed;
};

class DiskVerifier {
public:
    using ProgressCallback = std::function<void(const Progress&)>;

    static VerificationResult verifySampling(
        const DiskManagement::Disk& disk,
        double samplingPercentage = 0.10,
        double passThreshold = 0.01,
        ProgressCallback onProgress = nullptr
    );

    static VerificationResult verifyFull(
        const DiskManagement::Disk& disk,
        ProgressCallback onProgress = nullptr
    );
};

#endif // DISK_VERIFIER_H

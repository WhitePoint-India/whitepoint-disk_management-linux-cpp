
#include <stdexcept>
#include <operations.hpp>

namespace DiskManagement {

// Progress
Progress::Progress(
    unsigned long long bytesProcessed,
    unsigned long long totalBytes
) : _bytesProcessed(bytesProcessed), _totalBytes(totalBytes) {
    
}

unsigned long long Progress::bytesProcessed() const { return _bytesProcessed; }
unsigned long long Progress::totalBytes() const { return _totalBytes; }

double Progress::fractionCompleted() const {
    if (_totalBytes == 0) return 0.0;
    return static_cast<double>(_bytesProcessed) / static_cast<double>(_totalBytes);
}

double Progress::percentageCompleted() const {
    return fractionCompleted() * 100.0;
}

// Delegate
bool DiskDeleteMethod::Delegate::shouldCancel() { return false; }

// Constructor
DiskDeleteMethod::DiskDeleteMethod(const std::string& id) : id(id) {

}

const std::string& DiskDeleteMethod::getID() const {
    return id;
}

void DiskDeleteMethod::deleteDisk(Disks::ATADisk& disk, DiskDeleteMethod::Delegate& delegate) {
    delegate.onError(getTitle() + " does not support ATA disks");
}

void DiskDeleteMethod::deleteDisk(Disks::NVMeDisk& disk, DiskDeleteMethod::Delegate& delegate) {
    delegate.onError(getTitle() + " does not support NVMe disks");
}

void DiskDeleteMethod::deleteDisk(Disks::USBDisk& disk, DiskDeleteMethod::Delegate& delegate) {
    delegate.onError(getTitle() + " does not support USB disks");
}

} // namespace DiskManagement